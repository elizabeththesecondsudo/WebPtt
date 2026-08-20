add_library(
    WebPttStt
    STATIC
        Subscriber.cpp
)

target_include_directories(
    WebPttStt
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    WebPttStt
    PUBLIC
        project_options
        whisper
)
