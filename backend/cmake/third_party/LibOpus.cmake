include_guard(GLOBAL)

set(OPUS_BUILD_PROGRAMS OFF CACHE BOOL "Disable libopus programs" FORCE)
set(OPUS_BUILD_TESTING OFF CACHE BOOL "Disable libopus tests" FORCE)

FetchContent_Declare(
    libopus
    GIT_REPOSITORY https://github.com/xiph/opus.git
    GIT_TAG v1.5.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(libopus)
