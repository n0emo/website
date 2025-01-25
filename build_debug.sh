#!/bin/sh

set -xe

cflags="-Wall -Wextra -Isrc -g"
sources=$(find src -name "*.c")

mkdir -p build
clang $cflags -DLOG_WITH_FILE $sources -o build/server
