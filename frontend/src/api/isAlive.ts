import { apiClient } from "./client";

export async function fetchIsAlive(): Promise<boolean> {
  const response = await apiClient.get<string>("/isAlive", {
    responseType: "text",
  });

  return response.status === 200;
}
