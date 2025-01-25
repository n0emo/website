#!/bin/sh

cflags="-Wall -Wextra -Isrc"
sources=$(find src -name "*.c")

mkdir -p build
clang $cflags -DLOG_WITH_FILE $sources -o build/server
