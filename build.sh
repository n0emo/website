#!/bin/sh

mkdir -p build
clang -Wall -Wextra src/*.c -g -o build/server
