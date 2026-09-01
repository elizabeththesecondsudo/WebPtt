import { toast } from "react-toastify";
import { useAppStore } from "./useAppStore";

export function useCopySessionId(sessionId: string | null) {
  const copied = useAppStore((state) => state.copiedSessionId);
  const setCopied = useAppStore((state) => state.setCopiedSessionId);

  const copy = async () => {
    if (!sessionId) return;
    try {
      await navigator.clipboard.writeText(sessionId);
      setCopied(true);
      toast.success("Session ID copied");
      window.setTimeout(() => setCopied(false), 1_500);
    } catch {
      toast.error("Could not copy the session ID");
    }
  };

  return { copied, copy };
}
