#!/bin/sh

mkdir -p build
clang -Wall -Wextra -DLOG_WITH_FILE -fsanitize=undefined src/*.c -g -o build/server
#clang -Wall -Wextra src/*.c -g -o build/server
