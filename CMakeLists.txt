
cmake_minimum_required(VERSION 3.20)

project(opusC LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# GLAD
add_library(glad STATIC
    src/glad.c
)

target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

# Executable
add_executable(opusC
    main.cpp
    src/audio.cpp
    games/pong.cpp
)

# Header directories
target_include_directories(opusC PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/meshes
    ${CMAKE_CURRENT_SOURCE_DIR}/shader
    ${CMAKE_CURRENT_SOURCE_DIR}/object
    ${CMAKE_CURRENT_SOURCE_DIR}/games
    ${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# ImGui sources
target_sources(opusC PRIVATE
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Assets
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# Linking
target_link_libraries(opusC PRIVATE
    glad
    glfw
    glm::glm
)

target_compile_definitions(opusC PRIVATE GL_SILENCE_DEPRECATION)