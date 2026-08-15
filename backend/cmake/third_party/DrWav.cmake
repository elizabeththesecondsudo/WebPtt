include_guard(GLOBAL)

FetchContent_Declare(
    dr_wav
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
    GIT_TAG wav-0.14.5
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(dr_wav)

add_library(dr_wav_headers INTERFACE)
target_include_directories(
    dr_wav_headers
    INTERFACE
        ${dr_wav_SOURCE_DIR}
)
