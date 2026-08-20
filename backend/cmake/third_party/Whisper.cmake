include(FetchContent)

FetchContent_Declare(
    whisper
    GIT_REPOSITORY https://github.com/ggml-org/whisper.cpp.git
    GIT_TAG        v1.9.3
)

set(WHISPER_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(WHISPER_BUILD_TESTS    OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(whisper)
