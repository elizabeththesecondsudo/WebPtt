include_guard(GLOBAL)

include(FetchContent)

include(${CMAKE_CURRENT_LIST_DIR}/third_party/OpenSSL.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/Threads.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/Boost.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/LibDataChannel.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/LibOpus.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/Spdlog.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/Glaze.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/DrWav.cmake)

# The SoundTouch submodule does not provide a CMake build system.
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/SoundTouch
                 ${CMAKE_CURRENT_BINARY_DIR}/soundtouch)
