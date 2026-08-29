import { useEffect, useRef, useState } from "react";
import { Phone, Mic, MicOff } from "lucide-react";
import axios from "axios";
import { createSession } from "./api/createSession";
import { useIsAlive } from "./hooks/useIsAlive";
import { useBackendWebSocket } from "./hooks/useBackendWebSocket";
import { useMicrophone } from "./hooks/useMicrophone";
import { usePeerConnection } from "./hooks/usePeerConnection";

function App() {
  const { data: isAlive, isPending, isError } = useIsAlive();
  const websocket = useBackendWebSocket();
  const microphone = useMicrophone();
  const remoteAudioRef = useRef<HTMLAudioElement>(null);
  const [targetSessionId, setTargetSessionId] = useState("");
  const [bridgeId, setBridgeId] = useState<string | null>(null);
  const [callError, setCallError] = useState<string | null>(null);
  const [isCalling, setIsCalling] = useState(false);
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

  const startCall = async () => {
    const sessionId = websocket.sessionId;
    const targetId = targetSessionId.trim();
    if (!sessionId || !targetId || sessionId === targetId) return;

    setIsCalling(true);
    setCallError(null);
    try {
      const bridge = await createSession(sessionId, targetId);
      setBridgeId(bridge.bridge_id_);
    } catch (error) {
      setBridgeId(null);
      if (axios.isAxiosError<{ error_?: string }>(error)) {
        setCallError(error.response?.data?.error_ ?? error.message);
      } else {
        setCallError(error instanceof Error ? error.message : "Call failed");
      }
    } finally {
      setIsCalling(false);
    }
  };

  const canCall =
    websocket.status === "connected" &&
    Boolean(websocket.sessionId) &&
    targetSessionId.trim().length > 0 &&
    targetSessionId.trim() !== websocket.sessionId &&
    !isCalling &&
    !bridgeId;

  return (
    <main className="flex min-h-screen items-center justify-center bg-slate-950 p-6 text-slate-100">
      <section className="w-full max-w-lg space-y-5 rounded-2xl border border-slate-800 bg-slate-900 p-6 shadow-xl">
        <div>
          <h1 className="text-xl font-semibold">WebPTT connection</h1>
          <p className="mt-1 text-sm text-slate-400">C++ backend status</p>
        </div>

        <dl className="divide-y divide-slate-800 rounded-xl border border-slate-800">
          <div className="flex items-center justify-between p-4">
            <dt className="text-sm text-slate-400">HTTP API</dt>
            <dd className="text-sm font-medium">
              {isPending
                ? "Checking…"
                : !isError && isAlive
                  ? "Connected"
                  : "Disconnected"}
            </dd>
          </div>
          <div className="flex items-center justify-between p-4">
            <dt className="text-sm text-slate-400">WebSocket</dt>
            <dd className="text-sm font-medium capitalize">
              {websocket.status}
            </dd>
          </div>
          <div className="flex items-center justify-between gap-4 p-4">
            <dt className="shrink-0 text-sm text-slate-400">Session ID</dt>
            <dd className="truncate font-mono text-xs text-slate-300">
              {websocket.sessionId ?? "Waiting for backend…"}
            </dd>
          </div>
          <div className="flex items-center justify-between p-4">
            <dt className="text-sm text-slate-400">WebRTC audio</dt>
            <dd className="text-sm font-medium capitalize">{peer.status}</dd>
          </div>
        </dl>

        <audio
          ref={remoteAudioRef}
          autoPlay
          playsInline
          muted={microphone.isCapturing}
        />

        <div className="space-y-3 border-t border-slate-800 pt-5">
          <label
            htmlFor="target-session-id"
            className="block text-sm font-medium text-slate-300"
          >
            Call another session
          </label>
          <input
            id="target-session-id"
            type="text"
            value={targetSessionId}
            disabled={Boolean(bridgeId)}
            onChange={(event) => {
              setTargetSessionId(event.target.value);
              setCallError(null);
            }}
            onKeyDown={(event) => {
              if (event.key === "Enter" && canCall) void startCall();
            }}
            placeholder="Paste target session ID"
            className="w-full rounded-xl border border-slate-700 bg-slate-950 px-4 py-3 font-mono text-xs text-slate-100 outline-none transition placeholder:text-slate-600 focus:border-emerald-500 disabled:cursor-not-allowed disabled:opacity-60"
          />
          <button
            type="button"
            disabled={!canCall}
            onClick={() => void startCall()}
            className="flex w-full items-center justify-center gap-2 rounded-xl bg-sky-500 px-4 py-3 text-sm font-semibold text-slate-950 transition hover:bg-sky-400 disabled:cursor-not-allowed disabled:opacity-50"
          >
            <Phone aria-hidden="true" className="size-4" />
            {isCalling ? "Connecting call…" : bridgeId ? "Call connected" : "Start call"}
          </button>
          <p
            className={`text-center text-xs ${callError ? "text-rose-400" : "text-slate-400"}`}
            role="status"
          >
            {callError ??
              (bridgeId
                ? `Bridge ${bridgeId} is active.`
                : "Enter the session ID shown on the other device.")}
          </p>
        </div>

        <div className="space-y-3 border-t border-slate-800 pt-5">
          <button
            type="button"
            disabled={microphone.isStarting}
            onClick={
              microphone.isCapturing ? microphone.stop : microphone.start
            }
            className={`flex w-full items-center justify-center gap-2 rounded-xl px-4 py-3 text-sm font-semibold transition disabled:cursor-wait disabled:opacity-60 ${
              microphone.isCapturing
                ? "bg-rose-500 text-white hover:bg-rose-400"
                : "bg-emerald-500 text-slate-950 hover:bg-emerald-400"
            }`}
          >
            {microphone.isCapturing ? (
              <MicOff aria-hidden="true" className="size-4" />
            ) : (
              <Mic aria-hidden="true" className="size-4" />
            )}
            {microphone.isStarting
              ? "Requesting microphone…"
              : microphone.isCapturing
                ? "Stop microphone"
                : "Start microphone"}
          </button>

          <p className="text-center text-xs text-slate-400" role="status">
            {microphone.error ??
              (microphone.isCapturing
                ? "Microphone audio is being captured."
                : "Microphone access starts only when you press the button.")}
          </p>
        </div>
      </section>
    </main>
  );
}

export default App;
