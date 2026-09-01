from dataclasses import dataclass
from difflib import get_close_matches
from enum import Enum

from voice_command_api.constants import COMMAND_NAMES, MIX_FILES

NUMBER_WORDS = {
    "zero": 0.0,
    "one": 1.0,
    "two": 2.0,
    "three": 3.0,
    "four": 4.0,
    "for": 4.0,
    "five": 5.0,
    "six": 6.0,
    "seven": 7.0,
    "eight": 8.0,
    "nine": 9.0,
    "ten": 10.0,
}


class CommandType(Enum):
    PITCHSHIFT = "pitchshift"
    TEMPO = "tempo"
    GAIN = "gain"
    MIX = "mix"


@dataclass(slots=True)
class Command:
    type: CommandType
    value: float | str


def _closest_match(value: str, choices: list[str], label: str) -> str:
    normalized = value.lower().strip(" .,!?")
    if normalized in choices:
        return normalized
    matches = get_close_matches(normalized, choices, n=1, cutoff=0.5)
    if not matches:
        raise ValueError(f"Unknown {label}: {normalized}")
    return matches[0]


def parse_number(value: str) -> float:
    value = value.lower().strip(" .,!?")
    if value in NUMBER_WORDS:
        return NUMBER_WORDS[value]
    if value.startswith("minus "):
        positive_value = value.removeprefix("minus ").strip()
        if positive_value in NUMBER_WORDS:
            return -NUMBER_WORDS[positive_value]
        try:
            return -float(positive_value)
        except ValueError as error:
            raise ValueError(f"Expected numeric value, got: {value}") from error
    try:
        return float(value)
    except ValueError as error:
        raise ValueError(f"Expected numeric value, got: {value}") from error


def parse_command(text: str) -> Command:
    normalized_text = text.lower().strip(" .!?\n\t")
    parts = normalized_text.split(maxsplit=1)
    if len(parts) != 2:
        raise ValueError(f"Expected '<command> <value>', got: {normalized_text}")

    raw_command, raw_value = parts
    command_name = _closest_match(raw_command, COMMAND_NAMES, "command")
    value = raw_value.strip(" .,!?")
    if not value:
        raise ValueError(f"Missing value for command: {command_name}")
    if command_name == CommandType.MIX.value:
        return Command(CommandType.MIX, _closest_match(value, MIX_FILES, "mix file"))
    return Command(CommandType(command_name), parse_number(value))
