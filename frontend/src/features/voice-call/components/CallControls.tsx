import { PhoneCall, PhoneOff } from "lucide-react";

export type CallControlsProps = {
  targetSessionId: string;
  isActive: boolean;
  canCall: boolean;
  isCalling: boolean;
  isHangingUp: boolean;
  isIncomingCall: boolean;
  onTargetSessionIdChange: (value: string) => void;
  onStartCall: () => void;
  onEndCall: () => void;
};

export function CallControls({
  targetSessionId,
  isActive,
  canCall,
  isCalling,
  isHangingUp,
  isIncomingCall,
  onTargetSessionIdChange,
  onStartCall,
  onEndCall,
}: CallControlsProps) {
  const buttonLabel = isHangingUp
    ? "Disconnecting call…"
    : isActive
      ? isIncomingCall
        ? "Leave call"
        : "End call"
      : isCalling
        ? "Connecting call…"
        : "Start call";

  return (
    <>
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
          disabled={isActive}
          onChange={(event) => onTargetSessionIdChange(event.target.value)}
          onKeyDown={(event) => {
            if (event.key === "Enter" && canCall) onStartCall();
          }}
          placeholder="Paste the other browser's session ID"
          className="w-full rounded-lg border border-black/30 bg-[#1e1f22] px-4 py-3 font-mono text-sm text-[#dbdee1] outline-none transition placeholder:font-sans placeholder:text-[#6d6f78] focus:border-indigo-400 disabled:cursor-not-allowed disabled:opacity-60"
        />
      </div>

      <button
        type="button"
        disabled={isActive ? isHangingUp : !canCall}
        onClick={isActive ? onEndCall : onStartCall}
        className={`flex w-full items-center justify-center gap-2 rounded-lg px-4 py-3 text-sm font-semibold transition disabled:cursor-not-allowed disabled:opacity-50 ${
          isActive
            ? "bg-rose-500 text-white hover:bg-rose-400"
            : "bg-indigo-500 text-white hover:bg-indigo-400"
        }`}
      >
        {isActive ? (
          <PhoneOff aria-hidden="true" className="size-4" />
        ) : (
          <PhoneCall aria-hidden="true" className="size-4" />
        )}
        {buttonLabel}
      </button>
      <p className="text-center text-xs text-[#949ba4]">
        {isActive
          ? isIncomingCall
            ? "You are the destination. Only you can hear the source audio."
            : "The destination is connected and can hear this session."
          : "Audio is routed only to the session you select."}
      </p>
    </>
  );
}
