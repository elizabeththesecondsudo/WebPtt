add_library(
    WebPttCore
    STATIC
        Settings.cpp
)

target_include_directories(
    WebPttCore
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    WebPttCore
    PUBLIC
        project_options
        glaze::glaze
)
