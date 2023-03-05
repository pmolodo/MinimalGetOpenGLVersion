#!/bin/bash

set -e
set -u

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

# sudo apt-get install --no-install-recommends libgl-dev

mkdir -p "${THIS_DIR}/_build/Release"
cd "${THIS_DIR}/_build/Release"
cmake -DCMAKE_BUILD_TYPE=Release ../..
cmake --build .