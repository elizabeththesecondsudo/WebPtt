include_guard(GLOBAL)

set(BOOST_INCLUDE_LIBRARIES
    asio
    beast
    uuid
    system
    CACHE STRING "Boost libraries to build" FORCE
)
set(BOOST_ENABLE_CMAKE ON CACHE BOOL "Use Boost's CMake build" FORCE)

FetchContent_Declare(
    boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.91.0-1/boost-1.91.0-1-cmake.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(boost)
