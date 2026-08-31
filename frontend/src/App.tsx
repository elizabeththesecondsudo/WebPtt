import { useEffect, useRef, useState } from "react";
import {
  Check,
  Copy,
  Headphones,
  Mic,
  MicOff,
  PhoneCall,
  PhoneOff,
  Radio,
} from "lucide-react";
import axios from "axios";
import { toast } from "react-toastify";
import { createSession } from "./api/createSession";
import { deleteSession } from "./api/deleteSession";
import { useIsAlive } from "./hooks/useIsAlive";
import { useBackendWebSocket } from "./hooks/useBackendWebSocket";
import { useMicrophone } from "./hooks/useMicrophone";
import { usePeerConnection } from "./hooks/usePeerConnection";
import { useSessionStatus } from "./hooks/useSessionStatus";

function App() {
  const { data: isAlive, isPending, isError } = useIsAlive();
  const websocket = useBackendWebSocket();
  const { data: sessionStatus } = useSessionStatus(websocket.sessionId);
  const microphone = useMicrophone();
  const stopMicrophone = microphone.stop;
  const remoteAudioRef = useRef<HTMLAudioElement>(null);
  const hasSeenActiveBridgeRef = useRef(false);
  const [targetSessionId, setTargetSessionId] = useState("");
  const [bridgeId, setBridgeId] = useState<string | null>(null);
  const [isCalling, setIsCalling] = useState(false);
  const [isHangingUp, setIsHangingUp] = useState(false);
  const [copied, setCopied] = useState(false);
  const peer = usePeerConnection({
    microphoneStream: microphone.stream,
    websocketStatus: websocket.status,
    sendSignal: websocket.send,
    subscribeToSignals: websocket.subscribe,
  });

  useEffect(() => {
    if (remoteAudioRef.current) {
      remoteAudioRef.current.srcObject = peer.remoteStream;
    }
  }, [peer.remoteStream]);

  const serverBridgeId = sessionStatus?.active_
    ? sessionStatus.bridge_id_
    : null;
  const activeBridgeId = serverBridgeId ?? bridgeId;
  const isIncomingCall = Boolean(
    sessionStatus?.active_ &&
    websocket.sessionId &&
    sessionStatus.target_session_id_ === websocket.sessionId,
  );

  useEffect(() => {
    if (sessionStatus?.active_) {
      hasSeenActiveBridgeRef.current = true;
      return;
    }
    if (hasSeenActiveBridgeRef.current) {
      hasSeenActiveBridgeRef.current = false;
      setBridgeId(null);
    }
  }, [sessionStatus]);

  useEffect(() => {
    if (isIncomingCall) stopMicrophone();
  }, [isIncomingCall, stopMicrophone]);

  useEffect(() => {
    if (microphone.error) toast.error(microphone.error);
  }, [microphone.error]);

  const startCall = async () => {
    const sessionId = websocket.sessionId;
    const targetId = targetSessionId.trim();
    if (!sessionId || !targetId || sessionId === targetId) return;

    setIsCalling(true);
    try {
      const bridge = await createSession(sessionId, targetId);
      setBridgeId(bridge.bridge_id_);
      toast.success("Call connected");
    } catch (error) {
      setBridgeId(null);
      if (axios.isAxiosError<{ error_?: string }>(error)) {
        toast.error(error.response?.data?.error_ ?? error.message);
      } else {
        toast.error(error instanceof Error ? error.message : "Call failed");
      }
    } finally {
      setIsCalling(false);
    }
  };

  const endCall = async () => {
    if (!activeBridgeId) return;

    setIsHangingUp(true);
    try {
      await deleteSession(activeBridgeId);
      setBridgeId(null);
      microphone.stop();
      toast.success("Call ended");
    } catch (error) {
      if (axios.isAxiosError<{ error_?: string }>(error)) {
        toast.error(error.response?.data?.error_ ?? error.message);
      } else {
        toast.error(
          error instanceof Error ? error.message : "Hang up failed",
        );
      }
    } finally {
      setIsHangingUp(false);
    }
  };

  const canCall =
    websocket.status === "connected" &&
    Boolean(websocket.sessionId) &&
    targetSessionId.trim().length > 0 &&
    targetSessionId.trim() !== websocket.sessionId &&
    !isCalling &&
    !activeBridgeId;

  const copySessionId = async () => {
    if (!websocket.sessionId) return;
    try {
      await navigator.clipboard.writeText(websocket.sessionId);
      setCopied(true);
      toast.success("Session ID copied");
      window.setTimeout(() => setCopied(false), 1_500);
    } catch {
      toast.error("Could not copy the session ID");
    }
  };

  const callStatus = activeBridgeId
    ? isIncomingCall
      ? "Incoming voice connected"
      : "Voice connected"
    : isCalling
      ? "Connecting…"
      : "Ready to connect";

  return (
    <main className="flex min-h-screen items-center justify-center bg-[#1e1f22] p-4 text-slate-100 sm:p-8">
      <section className="w-full max-w-xl overflow-hidden rounded-2xl border border-white/5 bg-[#2b2d31] shadow-2xl shadow-black/40">
        <div className="flex items-center gap-3 border-b border-black/20 bg-[#313338] px-6 py-5">
          <div className="grid size-11 place-items-center rounded-full bg-indigo-500 shadow-lg shadow-indigo-950/30">
            <Radio aria-hidden="true" className="size-5" />
          </div>
          <div className="min-w-0 flex-1">
            <h1 className="font-semibold tracking-tight">WebPTT Voice</h1>
            <p className="text-sm text-[#b5bac1]">
              Private browser-to-browser audio
            </p>
          </div>
          <span
            className={`size-2.5 rounded-full ${
              websocket.status === "connected" && !isError
                ? "bg-emerald-400 shadow-[0_0_10px_#34d399]"
                : "bg-rose-400"
            }`}
            title="Connection status"
          />
        </div>

        <div className="space-y-5 p-6">
          <div className="rounded-xl bg-[#232428] p-4">
            <div className="mb-2 flex items-center justify-between">
              <span className="text-xs font-semibold uppercase tracking-wider text-[#949ba4]">
                Your session
              </span>
              <span className="text-xs text-[#949ba4]">
                {isPending
                  ? "Checking…"
                  : !isError && isAlive
                    ? "Online"
                    : "Offline"}
              </span>
            </div>
            <div className="flex items-center gap-2">
              <code className="min-w-0 flex-1 truncate text-sm text-[#dbdee1]">
                {websocket.sessionId ?? "Waiting for backend…"}
              </code>
              <button
                type="button"
                disabled={!websocket.sessionId}
                onClick={() => void copySessionId()}
                className="grid size-8 shrink-0 place-items-center rounded-md text-[#b5bac1] transition hover:bg-white/10 hover:text-white disabled:opacity-40"
                aria-label="Copy session ID"
                title="Copy session ID"
              >
                {copied ? (
                  <Check className="size-4" />
                ) : (
                  <Copy className="size-4" />
                )}
              </button>
            </div>
          </div>

          <div
            className={`rounded-xl border p-5 transition ${
              activeBridgeId
                ? "border-emerald-400/30 bg-emerald-400/5"
                : "border-white/5 bg-[#313338]"
            }`}
          >
            <div className="flex items-center gap-3">
              <div
                className={`grid size-12 place-items-center rounded-full ${
                  activeBridgeId
                    ? "bg-emerald-500 text-white"
                    : "bg-[#404249] text-[#b5bac1]"
                }`}
              >
                {activeBridgeId ? (
                  <Headphones className="size-5" />
                ) : (
                  <PhoneCall className="size-5" />
                )}
              </div>
              <div className="min-w-0 flex-1">
                <p
                  className={`font-semibold ${activeBridgeId ? "text-emerald-400" : "text-white"}`}
                >
                  {callStatus}
                </p>
                <p className="truncate text-sm text-[#949ba4]">
                  {activeBridgeId
                    ? isIncomingCall
                      ? `Listening to ${sessionStatus?.source_session_id_}`
                      : `Sending audio to ${sessionStatus?.target_session_id_ || targetSessionId.trim()}`
                    : "Choose one destination session"}
                </p>
              </div>
              <span className="text-xs capitalize text-[#949ba4]">
                WebRTC {peer.status}
              </span>
            </div>
          </div>

          <audio
            ref={remoteAudioRef}
            autoPlay
            playsInline
            muted={Boolean(activeBridgeId && !isIncomingCall)}
          />

          <div className="space-y-2">
            <label
              htmlFor="target-session-id"
              className="text-xs font-semibold uppercase tracking-wider text-[#b5bac1]"
            >
              Destination session
            </label>
            <input
              id="target-session-id"
              type="text"
              value={targetSessionId}
              disabled={Boolean(activeBridgeId)}
              onChange={(event) => {
                setTargetSessionId(event.target.value);
              }}
              onKeyDown={(event) => {
                if (event.key === "Enter" && canCall) void startCall();
              }}
              placeholder="Paste the other browser's session ID"
              className="w-full rounded-lg border border-black/30 bg-[#1e1f22] px-4 py-3 font-mono text-sm text-[#dbdee1] outline-none transition placeholder:font-sans placeholder:text-[#6d6f78] focus:border-indigo-400 disabled:cursor-not-allowed disabled:opacity-60"
            />
          </div>

          <button
            type="button"
            disabled={activeBridgeId ? isHangingUp : !canCall}
            onClick={() => void (activeBridgeId ? endCall() : startCall())}
            className={`flex w-full items-center justify-center gap-2 rounded-lg px-4 py-3 text-sm font-semibold transition disabled:cursor-not-allowed disabled:opacity-50 ${
              activeBridgeId
                ? "bg-rose-500 text-white hover:bg-rose-400"
                : "bg-indigo-500 text-white hover:bg-indigo-400"
            }`}
          >
            {activeBridgeId ? (
              <PhoneOff aria-hidden="true" className="size-4" />
            ) : (
              <PhoneCall aria-hidden="true" className="size-4" />
            )}
            {isHangingUp
              ? "Disconnecting call…"
              : activeBridgeId
                ? isIncomingCall
                  ? "Leave call"
                  : "End call"
                : isCalling
                  ? "Connecting call…"
                  : "Start call"}
          </button>
          <p className="text-center text-xs text-[#949ba4]">
            {activeBridgeId
              ? isIncomingCall
                ? "You are the destination. Only you can hear the source audio."
                : "The destination is connected and can hear this session."
              : "Audio is routed only to the session you select."}
          </p>

          <div className="flex items-center justify-between rounded-xl bg-[#232428] p-3">
            <div className="min-w-0 px-1">
              <p className="text-sm font-medium text-[#dbdee1]">
                {isIncomingCall ? "Listening mode" : "Microphone"}
              </p>
              <p
                className="truncate text-xs text-[#949ba4]"
              >
                {isIncomingCall
                  ? "Source-only audio — your microphone is off"
                  : microphone.isCapturing
                    ? "Live — destination can hear you"
                    : "Muted"}
              </p>
            </div>
            <button
              type="button"
              disabled={microphone.isStarting || isIncomingCall}
              onClick={
                microphone.isCapturing ? microphone.stop : microphone.start
              }
              className={`grid size-11 place-items-center rounded-full transition disabled:cursor-wait disabled:opacity-60 ${
                microphone.isCapturing
                  ? "bg-[#404249] text-white hover:bg-[#4e5058]"
                  : "bg-rose-500 text-white hover:bg-rose-400"
              }`}
              aria-label={
                microphone.isCapturing ? "Mute microphone" : "Unmute microphone"
              }
              title={microphone.isCapturing ? "Mute" : "Unmute"}
            >
              {microphone.isCapturing ? (
                <Mic className="size-5" />
              ) : (
                <MicOff className="size-5" />
              )}
            </button>
          </div>
        </div>
      </section>
    </main>
  );
}

export default App;
