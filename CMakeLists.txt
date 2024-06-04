# Set the toolchain file for vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Project name
project(siudemka)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories("src")

# Collect all .cpp files in the src directory and its subdirectories
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add executable
add_executable(siudemka ${SOURCES})

# Find and link SDL2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(siudemka
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

# Find and link SDL2_image
find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(siudemka
    PRIVATE
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)

# Find and link SDL2_ttf
find_package(SDL2_ttf CONFIG REQUIRED)
target_link_libraries(siudemka
	PRIVATE
	$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

# Find and link spdlog
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(siudemka PRIVATE spdlog::spdlog)

# Find and link yaml-cpp
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(siudemka PRIVATE yaml-cpp::yaml-cpp)

# Set the output directory for the executable
set_target_properties(siudemka PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
