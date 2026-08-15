include_guard(GLOBAL)

FetchContent_Declare(
    glaze
    URL https://github.com/stephenberry/glaze/archive/refs/tags/v7.9.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(glaze)
