cmake_minimum_required(VERSION 3.5)

project(Fejnando LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(deps/glfw)

add_executable(${PROJECT_NAME}
    src/main.c
    src/shaders.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(${PROJECT_NAME} PRIVATE dl)

