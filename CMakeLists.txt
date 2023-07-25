cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project(RIO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Find the required packages.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

# Create static libraries required for other projects.
set(STATIC_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib/)
if (WIN32)
    # TODO: Add Windows libraries.
elseif (UNIX)
    set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype ${ASSIMP_LIBRARY})
endif (WIN32)

add_library(STB_IMAGE "${CMAKE_SOURCE_DIR}/src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)
add_library(GLAD "${CMAKE_SOURCE_DIR}/src/glad.c")
set(LIBS ${LIBS} GLAD)

# Load config files for root directory.
configure_file("cmake/root_directory.h.in" "cmake/root_directory.h")
include_directories("${CMAKE_BINARY_DIR}/cmake")

# Load all extern header files.
include_directories("${CMAKE_SOURCE_DIR}/include")

# Load all source and header files.
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${LIBS})
