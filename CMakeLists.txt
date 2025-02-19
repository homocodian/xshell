cmake_minimum_required(VERSION 3.13)

project(shell-starter-cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(CMAKE_CXX_EXTENSIONS OFF)        # Avoid compiler-specific extensions

# Add debug-specific definitions (e.g., to define DEBUG macro for debug builds)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)  # Define DEBUG macro in debug builds
endif()

add_executable(shell ${SOURCE_FILES})
