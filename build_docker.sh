#!/bin/bash

set -e
set -u

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

cd $THIS_DIR

docker build -t minimal_get_opengl_version --network=host --progress=plain .
mkdir -p _build/docker/Release
id=$(docker create minimal_get_opengl_version) && docker cp $id:/MinimalGetOpenGLVersion/_build/Release/GetOpenGLVersion _build/docker/Release/GetOpenGLVersion && docker rm -v $id

# To create an archive:
# VER=0.1.0 && pushd _build/docker/Release && tar -czvf MinimalGetOpenGLVersion.v${VER}.manylinux-x86_64.tar.gz GetOpenGLVersion && popd && mv _build/docker/Release/MinimalGetOpenGLVersion.v${VER}.manylinux-x86_64.tar.gz .
