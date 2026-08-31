import { useQuery } from "@tanstack/react-query";
import { fetchSessionStatus } from "../api/sessionStatus";

export function useSessionStatus(sessionId: string | null) {
  return useQuery({
    queryKey: ["sessionStatus", sessionId],
    queryFn: () => fetchSessionStatus(sessionId!),
    enabled: Boolean(sessionId),
    refetchInterval: 1_000,
    refetchIntervalInBackground: true,
    retry: false,
  });
}
