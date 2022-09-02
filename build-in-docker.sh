#!/bin/bash
set -e # fast-fail on first command with non-zero return
cd "$(dirname "$(realpath "$BASH_SOURCE")")" # cd to script directory
ARENA_SDK_CHECK_FILE=ArenaSDK_Linux_x64/Arena_SDK_Linux_x64.conf
if [[ ! -f ../${ARENA_SDK_CHECK_FILE} ]]; then
    cd ..
    echo "$(pwd)/${ARENA_SDK_CHECK_FILE} not found.
    Please extract Arena SDK Ubuntu 18.04, 64-bit in parent folder"
    exit 1
fi
docker build . -t u1804-cpp-dev
mkdir -p ../artifacts/build
docker run --rm -it --user $(id -u):$(id -g) -v $(pwd)/../ArenaSDK_Linux_x64:/opt/ArenaSDK_Linux_x64:ro -v $(pwd):/opt/src:ro -v $(pwd)/../artifacts:/opt/artifacts u1804-cpp-dev:latest sh -c "cd /opt/artifacts/build && cmake -G Ninja /opt/src -DCMAKE_INSTALL_PREFIX=/opt/artifacts && cmake --build . --target install"
