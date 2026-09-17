#!/bin/sh
# Cross-compile mrtp with the Open Watcom v2 compiler (MS-DOS target).
set -e

cd "$(dirname "$0")"

: "${WATCOM:=/home/mikolaj/open-watcom-v2}"
export WATCOM
export PATH="$WATCOM/binl64:$PATH"
export INCLUDE="$WATCOM/lh"

cmake -DCMAKE_CXX_COMPILER="$WATCOM/binl64/wcl386" -B build-watcom-dos -DCMAKE_SYSTEM_NAME=DOS -DCMAKE_SYSTEM_PROCESSOR=x86 -DCMAKE_CXX_FLAGS_INIT=-bt=dos -DCMAKE_EXECUTABLE_SUFFIX=.exe
cmake --build build-watcom-dos
