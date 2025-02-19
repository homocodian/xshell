#!/bin/sh
#
# Use this script to run your program LOCALLY.
#
# Note: Changing this script WILL NOT affect how CodeCrafters runs your program.
#
# Learn more: https://codecrafters.io/program-interface

set -e # Exit early if any commands fail

# Copied from .codecrafters/compile.sh
#
# - Edit this to change how your program compiles locally
# - Edit .codecrafters/compile.sh to change how your program compiles remotely

# Check for the "debug" argument
CMAKE_BUILD_TYPE=""

# If "debug" is passed, set the build type to Debug
if [ "$1" = "debug" ]; then
  CMAKE_BUILD_TYPE="-DCMAKE_BUILD_TYPE=Debug"
  shift # Shift the argument to process the rest of the arguments (if any)
fi

(
  cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
  cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake $CMAKE_BUILD_TYPE
  cmake --build ./build
)

# Copied from .codecrafters/run.sh
#
# - Edit this to change how your program runs locally
# - Edit .codecrafters/run.sh to change how your program runs remotely
exec ./build/shell "$@"
