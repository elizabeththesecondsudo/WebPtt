import time

from fastapi import FastAPI, HTTPException, Request

from voice_command_api.audio import convert_pcm_for_whisper
from voice_command_api.commands import parse_command
from voice_command_api.config import settings
from voice_command_api.constants import INPUT_SAMPLE_RATE
from voice_command_api.transcriber import Transcriber

app = FastAPI()
transcriber = Transcriber(settings.model_path)


@app.post("/transcribe")
async def transcribe(request: Request):
    raw_pcm = await request.body()
    input_samples = len(raw_pcm) // 4
    try:
        pcm = convert_pcm_for_whisper(raw_pcm)
    except ValueError as error:
        raise HTTPException(status_code=400, detail=str(error)) from error

    audio_duration_ms = input_samples / INPUT_SAMPLE_RATE * 1000.0
    start = time.perf_counter()
    try:
        raw_text = transcriber.transcribe(pcm)
    except Exception as error:
        raise HTTPException(status_code=500, detail=f"Transcription failed: {error}") from error

    transcription_time_ms = (time.perf_counter() - start) * 1000.0
    try:
        command = parse_command(raw_text)
    except ValueError as error:
        raise HTTPException(
            status_code=422,
            detail={
                "error_": str(error),
                "raw_text_": raw_text,
                "processing_time_ms_": round(transcription_time_ms, 2),
            },
        ) from error

    return {
        "raw_text_": raw_text,
        "command_": command.type.value,
        "value_": command.value,
        "processing_time_ms_": round(transcription_time_ms, 2),
        "audio_duration_ms_": round(audio_duration_ms, 2),
        "input_samples_": input_samples,
        "whisper_samples_": int(pcm.size),
    }

