add_library(
    WebPttWebRtc
    STATIC
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
        Boost::uuid
        datachannel
        WebPttUtils
)
