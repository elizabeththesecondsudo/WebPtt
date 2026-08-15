cmake_minimum_required(VERSION 3.15)

project(WebPtt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

include(cmake/ProjectOptions.cmake)
include(cmake/Dependencies.cmake)

add_subdirectory(src/Api)

add_executable(
    WebPtt
    src/main.cpp
)

target_link_libraries(
    WebPtt
    PRIVATE
        project_options
        WebPttApi
        dr_wav_headers
)