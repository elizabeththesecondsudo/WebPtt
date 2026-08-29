import { useCallback, useEffect, useRef, useState } from "react";

export function useMicrophone() {
  const streamRef = useRef<MediaStream | null>(null);
  const [stream, setStream] = useState<MediaStream | null>(null);
  const [isStarting, setIsStarting] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const stop = useCallback(() => {
    console.info("[Microphone] stopping capture");
    streamRef.current?.getTracks().forEach((track) => track.stop());
    streamRef.current = null;
    setStream(null);
  }, []);

  const start = useCallback(async () => {
    if (streamRef.current || isStarting) return;

    setIsStarting(true);
    setError(null);

    try {
      console.info("[Microphone] requesting audio permission");
      const microphoneStream = await navigator.mediaDevices.getUserMedia({
        audio: {
          autoGainControl: true,
          echoCancellation: true,
          noiseSuppression: true,
        },
        video: false,
      });

      const audioTrack = microphoneStream.getAudioTracks()[0];
      console.info("[Microphone] capture started", {
        trackId: audioTrack?.id,
        settings: audioTrack?.getSettings(),
      });
      audioTrack?.addEventListener(
        "ended",
        () => {
          streamRef.current = null;
          setStream(null);
        },
        { once: true },
      );

      streamRef.current = microphoneStream;
      setStream(microphoneStream);
    } catch (cause) {
      console.error("[Microphone] failed to start capture", cause);
      setError(
        cause instanceof DOMException && cause.name === "NotAllowedError"
          ? "Microphone permission was denied."
          : "Could not access the microphone.",
      );
    } finally {
      setIsStarting(false);
    }
  }, [isStarting]);

  useEffect(() => stop, [stop]);

  return {
    stream,
    isCapturing: stream !== null,
    isStarting,
    error,
    start,
    stop,
  };
}
