const OPUS_BITRATE = 64_000;
const OPUS_FMTP = [
  "minptime=20",
  `maxaveragebitrate=${OPUS_BITRATE}`,
  "stereo=0",
  "sprop-stereo=0",
  "useinbandfec=1",
  "usedtx=0",
  "cbr=1",
].join(";");

export function configureOpusSdp(sdp: string) {
  const opusPayloadType = sdp.match(/^a=rtpmap:(\d+) opus\/48000(?:\/\d+)?\r?$/im)?.[1];
  if (!opusPayloadType) return sdp;

  const fmtpPattern = new RegExp(`^a=fmtp:${opusPayloadType} .*$`, "im");
  const fmtp = `a=fmtp:${opusPayloadType} ${OPUS_FMTP}`;
  if (fmtpPattern.test(sdp)) return sdp.replace(fmtpPattern, fmtp);

  const rtpmapPattern = new RegExp(`(^a=rtpmap:${opusPayloadType} .*$)`, "im");
  return sdp.replace(rtpmapPattern, `$1\r\n${fmtp}`);
}

export async function configureOpusSender(sender: RTCRtpSender) {
  const parameters = sender.getParameters();
  if (parameters.encodings.length === 0) parameters.encodings = [{}];
  parameters.encodings.forEach((encoding) => {
    encoding.maxBitrate = OPUS_BITRATE;
  });
  await sender.setParameters(parameters);
}
