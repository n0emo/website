#!/bin/sh

set -xe

cflags="-Wall -Wextra -Isrc -g -fsanitize=undefined -fsanitize=address"
sources=$(find src -name "*.c")

mkdir -p build
clang $cflags -DLOG_WITH_FILE $sources -o build/server
