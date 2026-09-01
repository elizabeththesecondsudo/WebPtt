import { Check, Copy } from "lucide-react";

export type SessionCardProps = {
  sessionId: string | null;
  healthStatus: string;
  copied: boolean;
  onCopy: () => void;
};

export function SessionCard({
  sessionId,
  healthStatus,
  copied,
  onCopy,
}: SessionCardProps) {
  return (
    <div className="rounded-xl bg-[#232428] p-4">
      <div className="mb-2 flex items-center justify-between">
        <span className="text-xs font-semibold uppercase tracking-wider text-[#949ba4]">
          Your session
        </span>
        <span className="text-xs text-[#949ba4]">{healthStatus}</span>
      </div>
      <div className="flex items-center gap-2">
        <code className="min-w-0 flex-1 truncate text-sm text-[#dbdee1]">
          {sessionId ?? "Waiting for backend…"}
        </code>
        <button
          type="button"
          disabled={!sessionId}
          onClick={onCopy}
          className="grid size-8 shrink-0 place-items-center rounded-md text-[#b5bac1] transition hover:bg-white/10 hover:text-white disabled:opacity-40"
          aria-label="Copy session ID"
          title="Copy session ID"
        >
          {copied ? <Check className="size-4" /> : <Copy className="size-4" />}
        </button>
      </div>
    </div>
  );
}
