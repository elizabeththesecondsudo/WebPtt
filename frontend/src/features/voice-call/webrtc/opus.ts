const OPUS_BITRATE = 64_000;
const OPUS_PAYLOAD_TYPE = "111";
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
  const negotiatedOpusPayloadType = sdp.match(
    /^a=rtpmap:(\d+) opus\/48000(?:\/\d+)?\r?$/im,
  )?.[1];
  if (!negotiatedOpusPayloadType) return sdp;

  if (negotiatedOpusPayloadType !== OPUS_PAYLOAD_TYPE) {
    const audioStart = sdp.search(/^m=audio\s/im);
    if (audioStart === -1) return sdp;

    const nextMediaOffset = sdp.slice(audioStart + 1).search(/^m=/m);
    const audioEnd =
      nextMediaOffset === -1 ? sdp.length : audioStart + 1 + nextMediaOffset;
    const audioSection = sdp.slice(audioStart, audioEnd);
    const placeholder = "__OPUS_PAYLOAD_TYPE__";
    const swapPayloadType = (value: string) =>
      value
        .replace(
          new RegExp(
            `(^m=audio\\s+\\S+\\s+\\S+.*(?:^|\\s))${negotiatedOpusPayloadType}(?=\\s|$)`,
            "gm",
          ),
          `$1${placeholder}`,
        )
        .replace(
          new RegExp(
            `(^m=audio\\s+\\S+\\s+\\S+.*(?:^|\\s))${OPUS_PAYLOAD_TYPE}(?=\\s|$)`,
            "gm",
          ),
          `$1${negotiatedOpusPayloadType}`,
        )
        .replaceAll(placeholder, OPUS_PAYLOAD_TYPE)
        .replace(
          new RegExp(
            `^(a=(?:rtpmap|fmtp|rtcp-fb):)${negotiatedOpusPayloadType}(?=[ \/])`,
            "gm",
          ),
          `$1${placeholder}`,
        )
        .replace(
          new RegExp(
            `^(a=(?:rtpmap|fmtp|rtcp-fb):)${OPUS_PAYLOAD_TYPE}(?=[ \/])`,
            "gm",
          ),
          `$1${negotiatedOpusPayloadType}`,
        )
        .replaceAll(placeholder, OPUS_PAYLOAD_TYPE);

    sdp =
      sdp.slice(0, audioStart) +
      swapPayloadType(audioSection) +
      sdp.slice(audioEnd);
  }

  const fmtpPattern = new RegExp(`^a=fmtp:${OPUS_PAYLOAD_TYPE} .*$`, "im");
  const fmtp = `a=fmtp:${OPUS_PAYLOAD_TYPE} ${OPUS_FMTP}`;
  if (fmtpPattern.test(sdp)) return sdp.replace(fmtpPattern, fmtp);

  const rtpmapPattern = new RegExp(
    `(^a=rtpmap:${OPUS_PAYLOAD_TYPE} .*$)`,
    "im",
  );
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
