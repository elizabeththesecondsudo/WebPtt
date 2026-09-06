import { Mic, MicOff, ShieldCheck } from "lucide-react";

export type MicrophoneControlProps = {
  isIncomingCall: boolean;
  isCapturing: boolean;
  isStarting: boolean;
  onStart: () => void;
  onStop: () => void;
  isPushToTalkEnabled: boolean;
  isPushToTalkPressed: boolean;
};

export function MicrophoneControl({
  isIncomingCall,
  isCapturing,
  isStarting,
  onStart,
  onStop,
  isPushToTalkEnabled,
  isPushToTalkPressed,
}: MicrophoneControlProps) {
  return (
    <div
      className={`flex items-center justify-between rounded-xl border p-3 transition-colors ${
        isPushToTalkPressed
          ? "border-violet-400/60 bg-violet-500/15"
          : "border-transparent bg-[#232428]"
      }`}
    >
      <div className="min-w-0 px-1">
        <p className="flex items-center gap-2 text-sm font-medium text-[#dbdee1]">
          {isPushToTalkPressed && (
            <ShieldCheck
              aria-hidden="true"
              className="size-4 text-violet-300"
            />
          )}
          {isIncomingCall
            ? "Listening mode"
            : isPushToTalkPressed
              ? "Private command active"
              : "Microphone"}
        </p>
        <p className="truncate text-xs text-[#949ba4]">
          {isIncomingCall
            ? "Source-only audio — your microphone is off"
            : isPushToTalkPressed
              ? "Release Ctrl + Space to finish"
              : isPushToTalkEnabled
                ? "Live — hold Ctrl + Space for private command"
                : isCapturing
                  ? "Live — destination can hear you"
                  : "Muted — unmute to use private command"}
        </p>
      </div>
      <button
        type="button"
        disabled={isStarting || isIncomingCall}
        onClick={isCapturing ? onStop : onStart}
        className={`grid size-11 place-items-center rounded-full transition disabled:cursor-wait disabled:opacity-60 ${
          isCapturing
            ? "bg-[#404249] text-white hover:bg-[#4e5058]"
            : "bg-rose-500 text-white hover:bg-rose-400"
        }`}
        aria-label={isCapturing ? "Mute microphone" : "Unmute microphone"}
        title={isCapturing ? "Mute" : "Unmute"}
      >
        {isCapturing ? (
          <Mic className="size-5" />
        ) : (
          <MicOff className="size-5" />
        )}
      </button>
    </div>
  );
}
