import type { ErrorRequestHandler, RequestHandler } from "express";
import { HttpError } from "../errors/http-error.js";

export const notFoundHandler: RequestHandler = (_request, response) => {
  response.status(404).json({ error: "Route not found" });
};

export const errorHandler: ErrorRequestHandler = (error: unknown, _request, response, _next) => {
  if (error instanceof HttpError) {
    response.status(error.status).json({ error: error.message });
    return;
  }

  console.error("Unhandled control plane error", error);
  response.status(500).json({ error: "Internal server error" });
};
