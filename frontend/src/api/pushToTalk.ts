import { z } from "zod";
import { apiClient } from "./client";

const pushToTalkResponseSchema = z.object({
  success_: z.boolean(),
});

async function sendPushToTalkCommand(
  path: "/ptt/start" | "/ptt/stop",
  bridgeId: string,
  sessionId: string,
): Promise<void> {
  const response = await apiClient.post(
    path,
    JSON.stringify({ bridge_id_: bridgeId, session_id_: sessionId }),
    { headers: { "Content-Type": "text/plain" } },
  );

  pushToTalkResponseSchema.parse(response.data);
}

export function startPushToTalk(
  bridgeId: string,
  sessionId: string,
): Promise<void> {
  return sendPushToTalkCommand("/ptt/start", bridgeId, sessionId);
}

export function stopPushToTalk(
  bridgeId: string,
  sessionId: string,
): Promise<void> {
  return sendPushToTalkCommand("/ptt/stop", bridgeId, sessionId);
}
