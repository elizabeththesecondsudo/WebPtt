import { env } from "../config/env.js";
import { HttpError } from "../errors/http-error.js";

interface UpstreamResponse {
  status: number;
  body: string;
  contentType: string | null;
}

class MediaPlaneService {
  async isAlive(): Promise<UpstreamResponse> {
    try {
      const response = await fetch(`${env.mediaPlaneUrl}/isAlive`);

      return {
        status: response.status,
        body: await response.text(),
        contentType: response.headers.get("content-type"),
      };
    } catch (error: unknown) {
      throw new HttpError(502, "C++ backend is unavailable", { cause: error });
    }
  }
}

export const mediaPlane = new MediaPlaneService();
