#!/bin/sh

set -xe

cflags="-Wall -Wextra -Isrc -O2"
sources=$(find src -name "*.c")

mkdir -p build
clang $cflags $sources -o ./build/server
