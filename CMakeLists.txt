cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project(RIO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Find the required packages.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

if (WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif (WIN32)

link_directories(${CMAKE_SOURCE_DIR}/lib)

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(assimp REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

# Create static libraries required for other projects.
set(STATIC_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib/)
if (WIN32)
    set(LIBS glfw3 opengl32 assimp-vc142-mt freetype irrKlang)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
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
include_directories("${CMAKE_SOURCE_DIR}/src/imgui")

# Load all source and header files.
function(get_source_file_recursive source_dir result)
    file(GLOB_RECURSE source "${source_dir}/*.cpp")
    file(GLOB_RECURSE header "${source_dir}/*.h" "${source_dir}/*.hpp")
    set(${result} ${source} ${header} PARENT_SCOPE)
endfunction()

set(SOURCES)
get_source_file_recursive("${CMAKE_SOURCE_DIR}/src" SOURCES)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
