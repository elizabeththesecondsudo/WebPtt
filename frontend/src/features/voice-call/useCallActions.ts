import { toast } from "react-toastify";
import { createSession } from "../../api/createSession";
import { deleteSession } from "../../api/deleteSession";
import { getErrorMessage } from "./getErrorMessage";
import { useAppStore } from "./useAppStore";

type UseCallActionsOptions = {
  sessionId: string | null;
  targetSessionId: string;
  activeBridgeId: string | null;
  stopMicrophone: () => void;
};

export function useCallActions({
  sessionId,
  targetSessionId,
  activeBridgeId,
  stopMicrophone,
}: UseCallActionsOptions) {
  const isCalling = useAppStore((state) => state.isCalling);
  const isHangingUp = useAppStore((state) => state.isHangingUp);
  const setBridgeId = useAppStore((state) => state.setBridgeId);
  const setIsCalling = useAppStore((state) => state.setIsCalling);
  const setIsHangingUp = useAppStore((state) => state.setIsHangingUp);

  const start = async () => {
    const targetId = targetSessionId.trim();
    if (!sessionId || !targetId || sessionId === targetId) return;
    setIsCalling(true);
    try {
      const bridge = await createSession(sessionId, targetId);
      setBridgeId(bridge.bridge_id_);
      toast.success("Call connected");
    } catch (error) {
      setBridgeId(null);
      toast.error(getErrorMessage(error, "Call failed"));
    } finally {
      setIsCalling(false);
    }
  };

  const end = async () => {
    if (!activeBridgeId) return;
    setIsHangingUp(true);
    try {
      await deleteSession(activeBridgeId);
      setBridgeId(null);
      stopMicrophone();
      toast.success("Call ended");
    } catch (error) {
      toast.error(getErrorMessage(error, "Hang up failed"));
    } finally {
      setIsHangingUp(false);
    }
  };

  return { isCalling, isHangingUp, start, end };
}
