INPUT_SAMPLE_RATE = 48_000
WHISPER_SAMPLE_RATE = 16_000

COMMAND_NAMES = ["pitchshift", "tempo", "gain", "mix"]
MIX_FILES = ["rain", "thunder", "alarm", "music"]
HOTWORDS = " ".join([*COMMAND_NAMES, *MIX_FILES])
INITIAL_PROMPT = (
    "Voice commands: pitchshift <number>, tempo <number>, gain <number>, mix <file name>. "
    "Available mix files: " + ", ".join(MIX_FILES) + "."
)
