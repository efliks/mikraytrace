#!/bin/sh
# Configure and build mrtp with GCC (Linux target).
set -e

cd "$(dirname "$0")"

cmake -B build-gcc
cmake --build build-gcc
