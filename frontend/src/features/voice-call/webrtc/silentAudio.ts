export function createSilentAudio() {
  const context = new AudioContext();
  const source = context.createConstantSource();
  const gain = context.createGain();
  const destination = context.createMediaStreamDestination();

  gain.gain.value = 0;
  source.connect(gain).connect(destination);
  source.start();

  const track = destination.stream.getAudioTracks()[0];
  return {
    track,
    close: () => {
      track.stop();
      source.stop();
      void context.close();
    },
  };
}
