import { useEffect, useRef, useState } from "react";
import { registerPeerEvents } from "./webrtc/peerEvents";
import { connectSignaling } from "./webrtc/signaling";
import { createSilentAudio } from "./webrtc/silentAudio";
import type {
  PeerConnectionStatus,
  SendSignal,
  SubscribeToSignals,
} from "./webrtc/types";
import type { WebSocketStatus } from "../../hooks/useBackendWebSocket";

export type { PeerConnectionStatus };

type UsePeerConnectionOptions = {
  microphoneStream: MediaStream | null;
  websocketStatus: WebSocketStatus;
  sendSignal: SendSignal;
  subscribeToSignals: SubscribeToSignals;
};

export function usePeerConnection({
  microphoneStream,
  websocketStatus,
  sendSignal,
  subscribeToSignals,
}: UsePeerConnectionOptions) {
  const audioSenderRef = useRef<RTCRtpSender | null>(null);
  const placeholderTrackRef = useRef<MediaStreamTrack | null>(null);
  const [status, setStatus] = useState<PeerConnectionStatus>("idle");
  const [remoteStream, setRemoteStream] = useState<MediaStream | null>(null);

  useEffect(() => {
    if (websocketStatus !== "connected") return;

    console.info("[WebRTC] creating peer connection");
    const peer = new RTCPeerConnection({ iceServers: [] });
    const remoteAudio = new MediaStream();
    // A silent initial track establishes the RTP source before microphone
    // permission is requested, allowing replaceTrack without renegotiation.
    const silentAudio = createSilentAudio();
    const transceiver = peer.addTransceiver(silentAudio.track, {
      direction: "sendrecv",
    });
    audioSenderRef.current = transceiver.sender;
    placeholderTrackRef.current = silentAudio.track;

    registerPeerEvents({
      peer,
      remoteAudio,
      sendSignal,
      setStatus,
      setRemoteStream,
    });
    const disconnectSignaling = connectSignaling({
      peer,
      sendSignal,
      subscribeToSignals,
      onStatusChange: setStatus,
    });

    return () => {
      console.info("[WebRTC] closing peer connection");
      disconnectSignaling();
      audioSenderRef.current = null;
      placeholderTrackRef.current = null;
      remoteAudio.getTracks().forEach((track) => track.stop());
      silentAudio.close();
      peer.close();
      setRemoteStream(null);
      setStatus("disconnected");
    };
  }, [sendSignal, subscribeToSignals, websocketStatus]);

  useEffect(() => {
    const sender = audioSenderRef.current;
    const track =
      microphoneStream?.getAudioTracks()[0] ?? placeholderTrackRef.current;
    if (!sender || !track) return;

    console.info("[WebRTC] replacing outgoing audio track", {
      source: microphoneStream ? "microphone" : "silent-placeholder",
      id: track.id,
      enabled: track.enabled,
      muted: track.muted,
      readyState: track.readyState,
      settings: track.getSettings(),
    });
    void sender.replaceTrack(track).then(
      () => console.info("[WebRTC] outgoing audio track replaced successfully"),
      (error) => {
        console.error("[WebRTC] replaceTrack failed", error);
        setStatus("failed");
      },
    );
  }, [microphoneStream]);

  return { status, remoteStream };
}
