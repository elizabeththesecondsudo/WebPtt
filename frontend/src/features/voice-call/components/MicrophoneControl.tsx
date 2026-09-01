import { Mic, MicOff } from "lucide-react";

export type MicrophoneControlProps = {
  isIncomingCall: boolean;
  isCapturing: boolean;
  isStarting: boolean;
  onStart: () => void;
  onStop: () => void;
};

export function MicrophoneControl({
  isIncomingCall,
  isCapturing,
  isStarting,
  onStart,
  onStop,
}: MicrophoneControlProps) {
  return (
    <div className="flex items-center justify-between rounded-xl bg-[#232428] p-3">
      <div className="min-w-0 px-1">
        <p className="text-sm font-medium text-[#dbdee1]">
          {isIncomingCall ? "Listening mode" : "Microphone"}
        </p>
        <p className="truncate text-xs text-[#949ba4]">
          {isIncomingCall
            ? "Source-only audio — your microphone is off"
            : isCapturing
              ? "Live — destination can hear you"
              : "Muted"}
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
