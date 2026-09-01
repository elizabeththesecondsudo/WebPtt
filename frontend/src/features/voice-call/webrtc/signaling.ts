import type {
  PeerConnectionStatus,
  SendSignal,
  SubscribeToSignals,
} from "./types";

type SignalingOptions = {
  peer: RTCPeerConnection;
  sendSignal: SendSignal;
  subscribeToSignals: SubscribeToSignals;
  onStatusChange: (status: PeerConnectionStatus) => void;
};

export function connectSignaling({
  peer,
  sendSignal,
  subscribeToSignals,
  onStatusChange,
}: SignalingOptions) {
  let chain = Promise.resolve();
  const pendingCandidates: RTCIceCandidateInit[] = [];

  const fail = (context: string, error: unknown) => {
    console.error(`[WebRTC] ${context}`, error);
    onStatusChange("failed");
  };

  const negotiate = () => {
    chain = chain
      .then(async () => {
        if (peer.signalingState !== "stable") return;
        const offer = await peer.createOffer();
        await peer.setLocalDescription(offer);
        if (peer.localDescription?.type === "offer") {
          sendSignal({ type_: "offer", sdp_: peer.localDescription.sdp });
        }
      })
      .catch((error) => fail("negotiation failed", error));
  };
  peer.addEventListener("negotiationneeded", negotiate);

  const unsubscribe = subscribeToSignals((message) => {
    chain = chain
      .then(async () => {
        if (message.type_ === "session_created") return;
        if (
          message.type_ === "candidate" ||
          message.type_ === "ice_candidate"
        ) {
          const candidate = {
            candidate: message.candidate_,
            sdpMid: message.sdp_mid_,
          };
          if (!peer.remoteDescription) {
            pendingCandidates.push(candidate);
            return;
          }
          await peer.addIceCandidate(candidate);
          return;
        }

        if (!("sdp_" in message)) return;

        if (message.type_ === "offer" && peer.signalingState !== "stable") {
          await peer.setLocalDescription({ type: "rollback" });
        }
        await peer.setRemoteDescription({
          type: message.type_,
          sdp: message.sdp_,
        });
        for (const candidate of pendingCandidates.splice(0)) {
          await peer.addIceCandidate(candidate);
        }
        if (message.type_ === "offer") {
          const answer = await peer.createAnswer();
          await peer.setLocalDescription(answer);
          if (peer.localDescription?.type === "answer") {
            sendSignal({ type_: "answer", sdp_: peer.localDescription.sdp });
          }
        }
      })
      .catch((error) => fail("failed to apply signaling message", error));
  });

  return () => {
    peer.removeEventListener("negotiationneeded", negotiate);
    unsubscribe();
  };
}
