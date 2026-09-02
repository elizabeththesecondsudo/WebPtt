add_library(
    WebPttAudio
    STATIC
        OpusTranscoder.cpp
)

target_include_directories(
    WebPttAudio
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    WebPttAudio
    PUBLIC
        project_options
        Opus::opus
)
