cmake_minimum_required(VERSION 3.15)

project(WebPtt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

include(cmake/ProjectOptions.cmake)
include(cmake/Dependencies.cmake)

configure_file(
    ${PROJECT_SOURCE_DIR}/settings-example.toml
    ${PROJECT_BINARY_DIR}/settings.toml
    COPYONLY
)

add_subdirectory(src/Api)
add_subdirectory(src/Core)
add_subdirectory(src/Stt)

add_executable(
    WebPtt
    src/main.cpp
)

target_link_libraries(
    WebPtt
    PRIVATE
        project_options
        WebPttApi
        WebPttCore
        WebPttStt
        dr_wav_headers
)
