#!/bin/env bash
export EMSDK_QUIET=1
source "/opt/emsdk/emsdk_env.sh"

echo "Building libraries"
embuilder build sdl2 sdl2_gfx sdl2_image sdl2_mixer sdl2_net sdl2_ttf sdl3 libpng zlib

make clean
make
       
