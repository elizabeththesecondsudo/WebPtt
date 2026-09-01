import axios from "axios";

export function getErrorMessage(error: unknown, fallback: string) {
  if (axios.isAxiosError<{ error_?: string }>(error)) {
    return error.response?.data?.error_ ?? error.message;
  }
  return error instanceof Error ? error.message : fallback;
}
