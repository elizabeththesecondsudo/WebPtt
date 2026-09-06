import { AppHeader } from "./components/AppHeader";
import { CallControls } from "./components/CallControls";
import { CallStatusCard } from "./components/CallStatusCard";
import { MicrophoneControl } from "./components/MicrophoneControl";
import { SessionCard } from "./components/SessionCard";
import { useCallController } from "./useCallController";

export function VoiceCallPage() {
  const { session, call, microphone, pushToTalk, peerStatus, remoteAudioRef } =
    useCallController();

  return (
    <main
      className={`flex min-h-screen items-center justify-center p-4 text-slate-100 transition-colors duration-200 sm:p-8 ${
        pushToTalk.isPressed ? "bg-[#16112b]" : "bg-[#1e1f22]"
      }`}
    >
      <section
        className={`w-full max-w-xl overflow-hidden rounded-2xl border bg-[#2b2d31] shadow-2xl transition duration-200 ${
          pushToTalk.isPressed
            ? "border-violet-400/70 shadow-violet-950/60 ring-2 ring-violet-400/20"
            : "border-white/5 shadow-black/40"
        }`}
      >
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
            isPushToTalkEnabled={pushToTalk.isEnabled}
            isPushToTalkPressed={pushToTalk.isPressed}
          />
        </div>
      </section>
    </main>
  );
}
