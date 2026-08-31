import { apiClient } from "./client";

export async function deleteSession(bridgeId: string): Promise<void> {
  // text/plain avoids a CORS preflight while still sending a JSON body.
  await apiClient.post(
    "/deleteSession",
    JSON.stringify({ bridge_id_: bridgeId }),
    { headers: { "Content-Type": "text/plain" } },
  );
}
