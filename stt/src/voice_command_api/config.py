import json
from dataclasses import dataclass
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[2]
CONFIG_PATH = PROJECT_ROOT / "config.json"


@dataclass(frozen=True, slots=True)
class Settings:
    model_path: Path
    host: str
    port: int


def load_settings(config_path: Path = CONFIG_PATH) -> Settings:
    with config_path.open(encoding="utf-8") as config_file:
        config = json.load(config_file)

    try:
        configured_path = Path(config["model"]["path"])
        host = config["server"]["host"]
        port = config["server"]["port"]
    except (KeyError, TypeError) as error:
        raise ValueError("config.json must define 'model.path', 'server.host', and 'server.port'") from error

    if not isinstance(host, str) or not host:
        raise ValueError("server.host must be a non-empty string")
    if not isinstance(port, int) or isinstance(port, bool) or not 1 <= port <= 65_535:
        raise ValueError("server.port must be an integer between 1 and 65535")

    model_path = configured_path if configured_path.is_absolute() else PROJECT_ROOT / configured_path
    return Settings(model_path=model_path, host=host, port=port)


settings = load_settings()
