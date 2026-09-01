import { useEffect, useRef } from "react";
import { toast } from "react-toastify";
import { useBackendWebSocket } from "../../hooks/useBackendWebSocket";
import { useIsAlive } from "../../hooks/useIsAlive";
import { useMicrophone } from "../../hooks/useMicrophone";
import { useSessionStatus } from "../../hooks/useSessionStatus";
import { getCallPresentation } from "./callPresentation";
import { useCallActions } from "./useCallActions";
import { useAppStore } from "./useAppStore";
import { useCopySessionId } from "./useCopySessionId";
import { usePeerConnection } from "./usePeerConnection";
import { useRemoteAudio } from "./useRemoteAudio";

export function useCallController() {
  const health = useIsAlive();
  const websocket = useBackendWebSocket();
  const { data: sessionStatus } = useSessionStatus(websocket.sessionId);
  const microphone = useMicrophone();
  const stopMicrophone = microphone.stop;
  const hasSeenActiveBridgeRef = useRef(false);
  const targetSessionId = useAppStore((state) => state.targetSessionId);
  const bridgeId = useAppStore((state) => state.bridgeId);
  const setTargetSessionId = useAppStore((state) => state.setTargetSessionId);
  const setBridgeId = useAppStore((state) => state.setBridgeId);
  const peer = usePeerConnection({
    microphoneStream: microphone.stream,
    websocketStatus: websocket.status,
    sendSignal: websocket.send,
    subscribeToSignals: websocket.subscribe,
  });
  const remoteAudioRef = useRemoteAudio(peer.remoteStream);
  const sessionIdCopy = useCopySessionId(websocket.sessionId);

  const serverBridgeId = sessionStatus?.active_
    ? sessionStatus.bridge_id_
    : null;
  const activeBridgeId = serverBridgeId ?? bridgeId;
  const isIncomingCall = Boolean(
    sessionStatus?.active_ &&
    websocket.sessionId &&
    sessionStatus.target_session_id_ === websocket.sessionId,
  );
  const callActions = useCallActions({
    sessionId: websocket.sessionId,
    targetSessionId,
    activeBridgeId,
    stopMicrophone,
  });

  useEffect(() => {
    if (sessionStatus?.active_) {
      hasSeenActiveBridgeRef.current = true;
      return;
    }
    if (hasSeenActiveBridgeRef.current) {
      hasSeenActiveBridgeRef.current = false;
      setBridgeId(null);
    }
  }, [sessionStatus, setBridgeId]);

  useEffect(() => {
    if (isIncomingCall) stopMicrophone();
  }, [isIncomingCall, stopMicrophone]);

  useEffect(() => {
    if (microphone.error) toast.error(microphone.error);
  }, [microphone.error]);

  const isActive = Boolean(activeBridgeId);
  const canCall =
    websocket.status === "connected" &&
    Boolean(websocket.sessionId) &&
    targetSessionId.trim().length > 0 &&
    targetSessionId.trim() !== websocket.sessionId &&
    !callActions.isCalling &&
    !isActive;
  const presentation = getCallPresentation({
    isActive,
    isIncoming: isIncomingCall,
    isCalling: callActions.isCalling,
    targetSessionId,
    sessionStatus,
  });

  return {
    session: {
      id: websocket.sessionId,
      copied: sessionIdCopy.copied,
      copy: sessionIdCopy.copy,
      healthStatus: health.isPending
        ? "Checking…"
        : !health.isError && health.data
          ? "Online"
          : "Offline",
      isConnected: websocket.status === "connected" && !health.isError,
    },
    call: {
      targetSessionId,
      setTargetSessionId,
      isActive,
      isIncoming: isIncomingCall,
      isCalling: callActions.isCalling,
      isHangingUp: callActions.isHangingUp,
      canCall,
      start: callActions.start,
      end: callActions.end,
      ...presentation,
    },
    microphone,
    peerStatus: peer.status,
    remoteAudioRef,
  };
}
