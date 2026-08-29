import { z } from "zod";
import { apiClient } from "./client";

const createSessionResponseSchema = z.object({
  bridge_id_: z.string(),
  session_id_: z.string(),
  target_session_id_: z.string(),
});

export type CreateSessionResponse = z.infer<
  typeof createSessionResponseSchema
>;

export async function createSession(
  sessionId: string,
  targetSessionId: string,
): Promise<CreateSessionResponse> {
  // text/plain keeps this cross-origin request simple, so the backend does not
  // need a separate CORS preflight route. The body itself is still JSON.
  const response = await apiClient.post(
    "/createSession",
    JSON.stringify({
      session_id_: sessionId,
      target_session_id_: targetSessionId,
    }),
    { headers: { "Content-Type": "text/plain" } },
  );

  return createSessionResponseSchema.parse(response.data);
}
