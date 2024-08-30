cmake_minimum_required(VERSION 3.18)
project(RIO LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Load all external library.
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(vendor/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(vendor/assimp)

add_library(glad vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_subdirectory(vendor/glm)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE vendor/stb)

add_library(json INTERFACE)
target_include_directories(json INTERFACE vendor/json/include)

add_subdirectory(vendor/pep-cprep)

file(GLOB IMGUI_SOURCES vendor/imgui/*.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC vendor/imgui)
target_link_libraries(imgui PUBLIC glfw)

# Include all source codes and link dependencies.
file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)

set(LIBS glfw glad assimp glm stb imgui json pep-cprep)
target_link_libraries(${PROJECT_NAME} ${LIBS})

# Load config for root directory.
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")
