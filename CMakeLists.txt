cmake_minimum_required(VERSION 3.5)

project(Fejnando LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(deps/glfw)
add_subdirectory(deps/cglm)

add_executable(${PROJECT_NAME}
    src/main_glfw.c
    src/shaders.c
    src/allocator.c
    src/game/sprites.c
    deps/glad/gl.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    deps/
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    cglm
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -O0
    -ggdb
)

