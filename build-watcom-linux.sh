#!/bin/sh
# Configure and build mrtp with the Open Watcom v2 compiler (Linux target).
set -e

cd "$(dirname "$0")"

: "${WATCOM:=/home/mikolaj/open-watcom-v2}"
export WATCOM
export PATH="$WATCOM/binl64:$PATH"
export INCLUDE="$WATCOM/lh"

cmake -DCMAKE_CXX_COMPILER="$WATCOM/binl64/wcl386" -B build-watcom
cmake --build build-watcom
