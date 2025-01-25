#!/bin/sh

set -xe

watchexec -r -w src -- "./build_debug.sh && ./build/server"
