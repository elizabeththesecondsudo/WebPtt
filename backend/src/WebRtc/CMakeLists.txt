add_library(
    WebPttWebRtc
    STATIC
        Bridge.cpp
        BridgeManager.cpp
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
