import { z } from "zod";

export const websocketMessageSchema = z.discriminatedUnion("type_", [
  z.object({
    type_: z.literal("session_created"),
    session_id_: z.string(),
  }),
  z.object({
    type_: z.enum(["offer", "answer"]),
    sdp_: z.string(),
  }),
  z.object({
    type_: z.enum(["candidate", "ice_candidate"]),
    candidate_: z.string(),
    sdp_mid_: z.string(),
  }),
]);

export type WebSocketMessage = z.infer<typeof websocketMessageSchema>;
