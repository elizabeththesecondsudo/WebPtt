add_library(
    WebPttUtils
    STATIC
        Net.cpp
)

target_include_directories(
    WebPttUtils
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    WebPttUtils
    PUBLIC
        project_options
        Boost::asio
        glaze::glaze
)
