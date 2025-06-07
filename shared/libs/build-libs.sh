#!/bin/env bash
export EMSDK_QUIET=1
source "/opt/emsdk/emsdk_env.sh"

echo "Building libraries"
embuilder build ALL

make clean
make
       
