import type { SessionStatus } from "../../api/sessionStatus";

type CallPresentationOptions = {
  isActive: boolean;
  isIncoming: boolean;
  isCalling: boolean;
  targetSessionId: string;
  sessionStatus?: SessionStatus;
};

export function getCallPresentation({
  isActive,
  isIncoming,
  isCalling,
  targetSessionId,
  sessionStatus,
}: CallPresentationOptions) {
  if (!isActive) {
    return {
      status: isCalling ? "Connecting…" : "Ready to connect",
      description: "Choose one destination session",
    };
  }
  return isIncoming
    ? {
        status: "Incoming voice connected",
        description: `Listening to ${sessionStatus?.source_session_id_}`,
      }
    : {
        status: "Voice connected",
        description: `Sending audio to ${sessionStatus?.target_session_id_ || targetSessionId.trim()}`,
      };
}
