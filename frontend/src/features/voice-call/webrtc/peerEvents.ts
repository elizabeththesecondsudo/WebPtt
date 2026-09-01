import type { Dispatch, SetStateAction } from "react";
import type { PeerConnectionStatus, SendSignal } from "./types";

type RegisterPeerEventsOptions = {
  peer: RTCPeerConnection;
  remoteAudio: MediaStream;
  sendSignal: SendSignal;
  setStatus: Dispatch<SetStateAction<PeerConnectionStatus>>;
  setRemoteStream: Dispatch<SetStateAction<MediaStream | null>>;
};

export function registerPeerEvents({
  peer,
  remoteAudio,
  sendSignal,
  setStatus,
  setRemoteStream,
}: RegisterPeerEventsOptions) {
  peer.addEventListener("connectionstatechange", () => {
    console.info("[WebRTC] connection state:", peer.connectionState);
    setStatus(peer.connectionState as PeerConnectionStatus);
  });

  peer.addEventListener("iceconnectionstatechange", () => {
    console.info("[WebRTC] ICE connection state:", peer.iceConnectionState);
  });
  peer.addEventListener("signalingstatechange", () => {
    console.info("[WebRTC] signaling state:", peer.signalingState);
  });
  peer.addEventListener("track", (event) => {
    console.info("[WebRTC] received remote track", {
      kind: event.track.kind,
      id: event.track.id,
      muted: event.track.muted,
      streams: event.streams.map((stream) => stream.id),
    });
    event.streams[0]?.getAudioTracks().forEach((track) => {
      if (!remoteAudio.getTrackById(track.id)) remoteAudio.addTrack(track);
    });
    if (event.streams.length === 0 && event.track.kind === "audio") {
      remoteAudio.addTrack(event.track);
    }
    setRemoteStream(new MediaStream(remoteAudio.getTracks()));
  });
  peer.addEventListener("icecandidate", (event) => {
    if (!event.candidate) {
      console.info("[WebRTC] ICE gathering complete");
      return;
    }
    sendSignal({
      type_: "candidate",
      candidate_: event.candidate.candidate,
      sdp_mid_: event.candidate.sdpMid ?? "0",
    });
  });
}
