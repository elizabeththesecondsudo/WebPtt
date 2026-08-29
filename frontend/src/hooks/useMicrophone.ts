import { useCallback, useEffect, useRef, useState } from "react";

export function useMicrophone() {
  const streamRef = useRef<MediaStream | null>(null);
  const [stream, setStream] = useState<MediaStream | null>(null);
  const [isStarting, setIsStarting] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const stop = useCallback(() => {
    streamRef.current?.getTracks().forEach((track) => track.stop());
    streamRef.current = null;
    setStream(null);
  }, []);

  const start = useCallback(async () => {
    if (streamRef.current || isStarting) return;

    setIsStarting(true);
    setError(null);

    try {
      const microphoneStream = await navigator.mediaDevices.getUserMedia({
        audio: {
          autoGainControl: true,
          echoCancellation: true,
          noiseSuppression: true,
        },
        video: false,
      });

      const audioTrack = microphoneStream.getAudioTracks()[0];
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
