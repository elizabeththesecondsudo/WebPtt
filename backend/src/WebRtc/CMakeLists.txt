add_library(
    WebPttWebRtc
    STATIC
        PeerConnectionManager.cpp
        Session.cpp
)

target_include_directories(
    WebPttWebRtc
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    WebPttWebRtc
    PUBLIC
        project_options
        datachannel
        spdlog::spdlog
        WebPttUtils
)
