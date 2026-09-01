import type { WebSocketMessage } from "../../../api/websocketMessages";

export type SendSignal = (message: WebSocketMessage) => boolean;
export type SubscribeToSignals = (
  listener: (message: WebSocketMessage) => void,
) => () => void;

export type PeerConnectionStatus =
  "idle" | "connecting" | "connected" | "disconnected" | "failed" | "closed";
