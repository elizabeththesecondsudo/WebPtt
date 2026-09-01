import { create } from "zustand";

export type AppState = {
  targetSessionId: string;
  bridgeId: string | null;
  isCalling: boolean;
  isHangingUp: boolean;
  copiedSessionId: boolean;
  setTargetSessionId: (targetSessionId: string) => void;
  setBridgeId: (bridgeId: string | null) => void;
  setIsCalling: (isCalling: boolean) => void;
  setIsHangingUp: (isHangingUp: boolean) => void;
  setCopiedSessionId: (copiedSessionId: boolean) => void;
};

export const useAppStore = create<AppState>()((set) => ({
  targetSessionId: "",
  bridgeId: null,
  isCalling: false,
  isHangingUp: false,
  copiedSessionId: false,
  setTargetSessionId: (targetSessionId) => set({ targetSessionId }),
  setBridgeId: (bridgeId) => set({ bridgeId }),
  setIsCalling: (isCalling) => set({ isCalling }),
  setIsHangingUp: (isHangingUp) => set({ isHangingUp }),
  setCopiedSessionId: (copiedSessionId) => set({ copiedSessionId }),
}));
