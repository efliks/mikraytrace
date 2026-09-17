#!/bin/sh
# Cross-compile mrtp with the DJGPP compiler (MS-DOS target).
set -e

cd "$(dirname "$0")"

: "${DJGPP:=/home/mikolaj/djgpp-12.1.0}"
export PATH="$DJGPP/bin:$PATH"
export GCC_EXEC_PREFIX="$DJGPP/lib/gcc/"
export DJDIR="$DJGPP/i586-pc-msdosdjgpp"

cmake -DCMAKE_CXX_COMPILER="$DJGPP/bin/i586-pc-msdosdjgpp-g++" -B build-djgpp -DUSE_OPENMP=OFF
cmake --build build-djgpp
