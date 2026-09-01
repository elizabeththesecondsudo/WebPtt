import { Headphones, PhoneCall } from "lucide-react";
import type { PeerConnectionStatus } from "../webrtc/types";

export type CallStatusCardProps = {
  isActive: boolean;
  title: string;
  description: string;
  peerStatus: PeerConnectionStatus;
};

export function CallStatusCard({
  isActive,
  title,
  description,
  peerStatus,
}: CallStatusCardProps) {
  return (
    <div
      className={`rounded-xl border p-5 transition ${
        isActive
          ? "border-emerald-400/30 bg-emerald-400/5"
          : "border-white/5 bg-[#313338]"
      }`}
    >
      <div className="flex items-center gap-3">
        <div
          className={`grid size-12 place-items-center rounded-full ${
            isActive
              ? "bg-emerald-500 text-white"
              : "bg-[#404249] text-[#b5bac1]"
          }`}
        >
          {isActive ? (
            <Headphones className="size-5" />
          ) : (
            <PhoneCall className="size-5" />
          )}
        </div>
        <div className="min-w-0 flex-1">
          <p
            className={`font-semibold ${
              isActive ? "text-emerald-400" : "text-white"
            }`}
          >
            {title}
          </p>
          <p className="truncate text-sm text-[#949ba4]">{description}</p>
        </div>
        <span className="text-xs capitalize text-[#949ba4]">
          WebRTC {peerStatus}
        </span>
      </div>
    </div>
  );
}
