add_library(
    RtpCpp
    STATIC
        RtpCpp/SmallVector.cpp
)
add_library(RtpCpp::RtpCpp ALIAS RtpCpp)

target_include_directories(
    RtpCpp
    SYSTEM PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(
    RtpCpp
    PUBLIC
        cxx_std_20
)

add_library(
    WebPttAudio
    STATIC
        OpusTranscoder.cpp
        Gain.cpp
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
