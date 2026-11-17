cmake_minimum_required(VERSION 3.20)

project(opusC LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_library(glad STATIC src/glad.c)

target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(meshes STATIC meshes/meshes.cpp)
add_library(utils STATIC utils/shader_utils.cpp)

target_include_directories(meshes PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/meshes
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(meshes PUBLIC glad)

add_executable(opusC main.cpp)

target_link_libraries(opusC PRIVATE meshes utils glfw glm::glm)