import { z } from "zod";
import { apiClient } from "./client";

const sessionStatusSchema = z.object({
  active_: z.boolean(),
  bridge_id_: z.string(),
  source_session_id_: z.string(),
  target_session_id_: z.string(),
});

export type SessionStatus = z.infer<typeof sessionStatusSchema>;

export async function fetchSessionStatus(
  sessionId: string,
): Promise<SessionStatus> {
  const response = await apiClient.post(
    "/sessionStatus",
    JSON.stringify({ session_id_: sessionId }),
    { headers: { "Content-Type": "text/plain" } },
  );
  return sessionStatusSchema.parse(response.data);
}
