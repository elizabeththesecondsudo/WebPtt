include_guard(GLOBAL)

set(ENABLE_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable dependency warnings as errors" FORCE)

FetchContent_Declare(
    spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(spdlog)
