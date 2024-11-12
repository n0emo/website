#!/bin/sh

mkdir -p build
clang -Wall -Wextra -DLOG_WITH_FILE src/*.c -g -o build/server
#clang -Wall -Wextra src/*.c -g -o build/server
