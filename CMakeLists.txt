cmake_minimum_required(VERSION 3.16)
project(FlappyBird)

set(CMAKE_CXX_STANDARD 17)

# Path to the raylib source you built
add_subdirectory(
    "C:/Users/Talha/Downloads/raylib-6.0/raylib-6.0"
    raylib_build
)

add_executable(FlappyBird src/main.cpp)

target_link_libraries(FlappyBird raylib)

if(EMSCRIPTEN)
    set_target_properties(FlappyBird PROPERTIES SUFFIX ".html")

    target_link_options(FlappyBird PRIVATE
        "--preload-file" "${CMAKE_SOURCE_DIR}/assets@assets"
    )
endif()