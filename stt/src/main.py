from voice_command_api.api import app
from voice_command_api.config import settings

if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, host=settings.host, port=settings.port)
