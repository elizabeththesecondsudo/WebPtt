cmake_minimum_required(VERSION 3.15)

project(WebPtt LANGUAGES CXX)

# --------------------------------------------------
# C++
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build dependencies as static libraries
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_library(project_options INTERFACE)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" ON)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(ENABLE_LSAN "Enable Leak Sanitizer" OFF)
include(Cmake/Sanitizers.cmake)
myproject_enable_sanitizers(project_options ${ENABLE_ASAN} ${ENABLE_LSAN} ${ENABLE_UBSAN} ${ENABLE_TSAN} FALSE)

include(FetchContent)

# --------------------------------------------------
# System dependencies
# --------------------------------------------------

set(OPENSSL_ROOT_DIR "/usr")
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# --------------------------------------------------
# Boost
# --------------------------------------------------

set(BOOST_INCLUDE_LIBRARIES
    asio
    beast
    uuid
    system
    CACHE STRING "" FORCE
)

set(BOOST_ENABLE_CMAKE ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.91.0-1/boost-1.91.0-1-cmake.zip
)

# --------------------------------------------------
# libdatachannel
# --------------------------------------------------

set(NO_EXAMPLES ON CACHE BOOL "" FORCE)
set(NO_TESTS ON CACHE BOOL "" FORCE)

set(NO_MEDIA ON CACHE BOOL "" FORCE)
set(NO_WEBSOCKET OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    libdatachannel
    GIT_REPOSITORY https://github.com/paullouisageneau/libdatachannel.git
    GIT_TAG v0.24.5
)

# --------------------------------------------------
# spdlog
# --------------------------------------------------

set(ENABLE_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.17.0.zip
)

# --------------------------------------------------
# glaze
# --------------------------------------------------

FetchContent_Declare(
    glaze
    URL https://github.com/stephenberry/glaze/archive/refs/tags/v7.9.1.zip
)

# --------------------------------------------------
# Fetch dependencies
# --------------------------------------------------

FetchContent_MakeAvailable(
    boost
    libdatachannel
    spdlog
    glaze
)

# --------------------------------------------------
# Backend
# --------------------------------------------------

add_executable(
    WebPtt
    src/main.cpp
)

target_link_libraries(
    WebPtt
    PRIVATE
        project_options
        Boost::asio
        Boost::beast
        Boost::uuid
        datachannel
        glaze::glaze
        spdlog::spdlog
        Threads::Threads
)