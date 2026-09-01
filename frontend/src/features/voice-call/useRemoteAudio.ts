import { useEffect, useRef } from "react";

export function useRemoteAudio(remoteStream: MediaStream | null) {
  const audioRef = useRef<HTMLAudioElement>(null);

  useEffect(() => {
    if (audioRef.current) audioRef.current.srcObject = remoteStream;
  }, [remoteStream]);

  return audioRef;
}
