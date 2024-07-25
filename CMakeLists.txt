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

file(GLOB IMGUI_SOURCES vendor/imgui/*.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC vendor/imgui)
target_link_libraries(imgui PUBLIC glfw)

# Load all source and header files.
function(get_source_file_recursive source_dir result)
    file(GLOB_RECURSE source "${source_dir}/*.cpp")
    file(GLOB_RECURSE header "${source_dir}/*.h" "${source_dir}/*.hpp")
    set(${result} ${source} ${header} PARENT_SCOPE)
endfunction()

set(SOURCES)
get_source_file_recursive("${CMAKE_SOURCE_DIR}/src" SOURCES)

add_executable(${PROJECT_NAME} ${SOURCES})

if (WIN32)
    set(LIBS glfw glad assimp glm imgui)
elseif (UNIX)
    message(FATAL_ERROR "RIO currently don't support unix-like system." )
endif (WIN32)

target_link_libraries(${PROJECT_NAME} ${LIBS})

# Load config for root directory.
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")
