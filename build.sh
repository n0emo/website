#!/bin/sh

mkdir -p build
clang -Wall -Wextra src/main.c -o build/server
