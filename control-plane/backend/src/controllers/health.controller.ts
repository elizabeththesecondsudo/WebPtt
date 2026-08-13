import type { Request, Response } from "express";
import { mediaPlane } from "../services/media-plane.service.js";

export async function isAlive(_request: Request, response: Response): Promise<void> {
  const upstreamResponse = await mediaPlane.isAlive();

  if (upstreamResponse.contentType !== null) {
    response.set("content-type", upstreamResponse.contentType);
  }

  response.status(upstreamResponse.status).send(upstreamResponse.body);
}
