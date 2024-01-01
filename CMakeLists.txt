cmake_minimum_required(VERSION 3.5)

project(Fejnando LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/glfw)
add_subdirectory(external/cglm)

add_executable(${PROJECT_NAME}
    src/main_glfw.c
    src/shaders.c
    src/allocator.c
    external/glad/gl.c
)

add_library(gamelib SHARED
    src/game.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    external/
    external/glad
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    cglm
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /Od
        /Wall
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -O0
        -ggdb
    )
endif()
