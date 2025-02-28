#!/bin/sh
#
# This script is used to compile your program on CodeCrafters
# 
# This runs before .codecrafters/run.sh
#
# Learn more: https://codecrafters.io/program-interface

# Exit early if any commands fail
set -e

git submodule update --init --recursive

if [ $? -eq 0 ]; then
  echo "Git submodules updated successfully."
else
  echo "Error updating Git submodules."
  exit 1 # Exit with an error code
fi

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
cmake --build ./build
