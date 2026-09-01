from pathlib import Path

import numpy as np
from faster_whisper import WhisperModel

from voice_command_api.constants import HOTWORDS, INITIAL_PROMPT


class Transcriber:
    def __init__(self, model_path: Path | str) -> None:
        self._model = WhisperModel(str(model_path), device="auto", compute_type="auto")

    def transcribe(self, pcm: np.ndarray) -> str:
        if pcm.size == 0:
            raise ValueError("PCM buffer is empty")
        if pcm.ndim != 1:
            raise ValueError("PCM must be mono")

        segments, _ = self._model.transcribe(
            pcm,
            language="en",
            beam_size=3,
            condition_on_previous_text=False,
            vad_filter=True,
            hotwords=HOTWORDS,
            initial_prompt=INITIAL_PROMPT,
        )
        return "".join(segment.text for segment in segments).strip()
