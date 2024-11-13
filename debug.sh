#!/bin/sh

set -xe

watchexec -r -w src -- "./build.sh && ./build/server"
