#!/bin/env bash
export EMSDK_QUIET=1
source "/opt/emsdk/emsdk_env.sh"

echo "Building libraries"
embuilder build libpng sdl2_mixer

make
