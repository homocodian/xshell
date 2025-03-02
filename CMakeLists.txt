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

# Add additional macro for DEBUG_TOKEN if it's passed via the script
if(DEFINED DEBUG_TOKEN)
    message(STATUS "DEBUG_TOKEN is defined.")
    add_definitions(-DDEBUG_TOKEN)  # Define DEBUG_TOKEN macro if set
endif()

add_subdirectory(external/hat-trie)

add_executable(shell ${SOURCE_FILES})

target_link_libraries(shell tsl_hat_trie)

target_include_directories(shell PRIVATE tsl::hat-trie)
