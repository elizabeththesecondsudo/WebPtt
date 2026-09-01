import { AppHeader } from "./components/AppHeader";
import { CallControls } from "./components/CallControls";
import { CallStatusCard } from "./components/CallStatusCard";
import { MicrophoneControl } from "./components/MicrophoneControl";
import { SessionCard } from "./components/SessionCard";
import { useCallController } from "./useCallController";

export function VoiceCallPage() {
  const { session, call, microphone, peerStatus, remoteAudioRef } =
    useCallController();

  return (
    <main className="flex min-h-screen items-center justify-center bg-[#1e1f22] p-4 text-slate-100 sm:p-8">
      <section className="w-full max-w-xl overflow-hidden rounded-2xl border border-white/5 bg-[#2b2d31] shadow-2xl shadow-black/40">
        <AppHeader isConnected={session.isConnected} />
        <div className="space-y-5 p-6">
          <SessionCard
            sessionId={session.id}
            healthStatus={session.healthStatus}
            copied={session.copied}
            onCopy={() => void session.copy()}
          />
          <CallStatusCard
            isActive={call.isActive}
            title={call.status}
            description={call.description}
            peerStatus={peerStatus}
          />
          <audio
            ref={remoteAudioRef}
            autoPlay
            playsInline
            muted={call.isActive && !call.isIncoming}
          />
          <CallControls
            targetSessionId={call.targetSessionId}
            isActive={call.isActive}
            canCall={call.canCall}
            isCalling={call.isCalling}
            isHangingUp={call.isHangingUp}
            isIncomingCall={call.isIncoming}
            onTargetSessionIdChange={call.setTargetSessionId}
            onStartCall={() => void call.start()}
            onEndCall={() => void call.end()}
          />
          <MicrophoneControl
            isIncomingCall={call.isIncoming}
            isCapturing={microphone.isCapturing}
            isStarting={microphone.isStarting}
            onStart={() => void microphone.start()}
            onStop={microphone.stop}
          />
        </div>
      </section>
    </main>
  );
}
