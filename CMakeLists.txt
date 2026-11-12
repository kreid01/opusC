cmake_minimum_required(VERSION 3.20)

project(opusC LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
add_library(glad STATIC src/glad.c)

# find_package(glm CONFIG REQUIRED)
# target_link_libraries(main PRIVATE glm::glm-header-only) 

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(opusC main.cpp)

target_include_directories(opusC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(opusC PRIVATE glad glfw)
