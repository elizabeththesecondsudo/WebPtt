import numpy as np
import soxr

from voice_command_api.constants import INPUT_SAMPLE_RATE, WHISPER_SAMPLE_RATE


def convert_pcm_for_whisper(raw_pcm: bytes) -> np.ndarray:
    if not raw_pcm:
        raise ValueError("PCM buffer is empty")
    if len(raw_pcm) % 4 != 0:
        raise ValueError("Invalid float32 PCM buffer size")

    pcm_48khz = np.frombuffer(raw_pcm, dtype="<f4")
    if not np.all(np.isfinite(pcm_48khz)):
        raise ValueError("PCM contains invalid float values")

    pcm_16khz = soxr.resample(pcm_48khz, INPUT_SAMPLE_RATE, WHISPER_SAMPLE_RATE, quality="HQ")
    return pcm_16khz.astype(np.float32, copy=False)
