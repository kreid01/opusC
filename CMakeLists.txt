
cmake_minimum_required(VERSION 3.20)

project(opusC LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# GLAD
add_library(glad STATIC src/glad.c)

target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Meshes
add_library(meshes STATIC meshes/meshes.cpp)

target_include_directories(meshes PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/meshes
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(meshes PUBLIC glad)

# Utils
add_library(utils STATIC utils/shader_utils.cpp)

# ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

# Executable
add_executable(opusC main.cpp)

#Audio
target_sources(opusC PRIVATE
    src/audio.cpp
)

target_include_directories(opusC PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio
)


target_sources(opusC PRIVATE
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(opusC PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(opusC PRIVATE
    meshes
    utils
    glfw
    glm::glm
)
