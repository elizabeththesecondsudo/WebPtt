include_guard(GLOBAL)

set(NO_EXAMPLES ON CACHE BOOL "Disable libdatachannel examples" FORCE)
set(NO_TESTS ON CACHE BOOL "Disable libdatachannel tests" FORCE)
set(NO_MEDIA ON CACHE BOOL "Disable libdatachannel media support" FORCE)
set(NO_WEBSOCKET OFF CACHE BOOL "Enable libdatachannel WebSocket support" FORCE)

FetchContent_Declare(
    libdatachannel
    GIT_REPOSITORY https://github.com/paullouisageneau/libdatachannel.git
    GIT_TAG v0.24.5
)

FetchContent_MakeAvailable(libdatachannel)
