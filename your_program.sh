#!/bin/sh

set -e # Exit early if any commands fail

# Initialize the CMake definitions
CMAKE_DEFINITIONS=""

# Process arguments
if [ "$1" = "debug" ]; then
  CMAKE_DEFINITIONS="-DCMAKE_BUILD_TYPE=Debug"
  shift # Shift to process the next argument
fi

# If "debug_token" argument is passed, add the DEBUG_TOKEN macro
if [ "$1" = "debug_token" ]; then
  CMAKE_DEFINITIONS="$CMAKE_DEFINITIONS -DDEBUG_TOKEN=ON"
  shift # Shift to process any further arguments (if any)
fi

(
  cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
  cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake $CMAKE_DEFINITIONS
  cmake --build ./build
)

exec ./build/shell "$@"
