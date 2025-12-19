#!/usr/bin/env bash

set -e

if [[ -t 1 ]]; then
  RED='\033[31m'
  GREEN='\033[32m'
  YELLOW='\033[33m'
  BLUE='\033[34m'
  RESET='\033[0m'
else
  RED=''
  GREEN=''
  YELLOW=''
  BLUE=''
  RESET=''
fi

CMAKE_DEFINITIONS=""
CMAKE_FLAGS=""
BUILD_TYPE_SET=false
SHOULD_RUN=true
SHOULD_CLEAN=false
SHOULD_CLEAN_ONLY=false

for arg in "$@"; do
  case "$arg" in
    --force)
      CMAKE_FLAGS+=" --fresh"
      ;;

    --debug)
      $BUILD_TYPE_SET && {
        echo "Error: multiple build types specified" >&2
        exit 1
      }
      BUILD_TYPE_SET=true
      CMAKE_DEFINITIONS+=" -DCMAKE_BUILD_TYPE=Debug"
      ;;

    --release)
      $BUILD_TYPE_SET && {
        echo "Error: multiple build types specified" >&2
        exit 1
      }
      BUILD_TYPE_SET=true
      CMAKE_DEFINITIONS+=" -DCMAKE_BUILD_TYPE=Release"
      ;;

    --debug-token)
      CMAKE_DEFINITIONS+=" -DDEBUG_TOKEN=ON"
      ;;

      --build)
      SHOULD_RUN=false
      ;;

    --clean)
      $SHOULD_CLEAN_ONLY && {
        echo "Error: --clean and --clean-only cannot be used together" >&2
        exit 1
      }
      SHOULD_CLEAN=true
      ;;

    --clean-only)
      $SHOULD_CLEAN && {
        echo "Error: --clean and --clean-only cannot be used together" >&2
        exit 1
      }
      SHOULD_CLEAN_ONLY=true
      ;;
  esac
done

if [ "$SHOULD_CLEAN_ONLY" = true ]; then
  echo -e "${RED}Cleaning build directory...${RESET}"
  rm -rf "$(dirname "$0")/build"
  exit 0
fi

if [ "$SHOULD_CLEAN" = true ]; then
  echo -e "${RED}Cleaning build directory...${RESET}"
  rm -rf "$(dirname "$0")/build"
fi

(
  cd "$(dirname "$0")"
  echo -e "${YELLOW}Configuring and building project...${RESET}"
  cmake $CMAKE_FLAGS -S . -B build $CMAKE_DEFINITIONS
  cmake --build build
)

if [ "$SHOULD_RUN" = true ]; then
  echo -e "${GREEN}Running shell executable...${RESET}"
  exec ./build/shell
fi