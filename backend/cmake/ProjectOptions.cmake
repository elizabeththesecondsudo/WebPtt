include_guard(GLOBAL)

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" ON)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(ENABLE_LSAN "Enable Leak Sanitizer" OFF)

add_library(project_options INTERFACE)

include(${CMAKE_CURRENT_LIST_DIR}/Sanitizers.cmake)
myproject_enable_sanitizers(
    project_options
    ${ENABLE_ASAN}
    ${ENABLE_LSAN}
    ${ENABLE_UBSAN}
    ${ENABLE_TSAN}
    FALSE
)
