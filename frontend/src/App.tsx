import { useEffect, useRef } from "react";
import { Mic, MicOff } from "lucide-react";
import { useIsAlive } from "./hooks/useIsAlive";
import { useBackendWebSocket } from "./hooks/useBackendWebSocket";
import { useMicrophone } from "./hooks/useMicrophone";
import { usePeerConnection } from "./hooks/usePeerConnection";

function App() {
  const { data: isAlive, isPending, isError } = useIsAlive();
  const websocket = useBackendWebSocket();
  const microphone = useMicrophone();
  const remoteAudioRef = useRef<HTMLAudioElement>(null);
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

        <audio ref={remoteAudioRef} autoPlay playsInline />

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
