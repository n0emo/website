profile := "debug"
sanitizers := "false"

cc := "clang"

common-cflags := "-std=gnu17 -Wall -Wextra -Wno-unused-parameter -fsanitize=thread " + \
    if sanitizers == "true" { "-fsanitize=undefined -fsanitize=address " } else { "" }
profile-cflags := if profile == "debug" {
    common-cflags + "-DLOG_WITH_FILE -g "
} else if profile == "release" {
    common-cflags + "-O2 "
} else {
    ""
}
website-cflags := profile-cflags + "-I./mew/include -I./website/src"
libmew-cflags := profile-cflags + "-I./mew/include -fPIC "

website-libs := "-L./build -lmew"

default: build

build: libmew
    mkdir -p build
    {{ cc }} {{ website-cflags }} -o ./build/server website/src/main.c {{ website-libs }}

libmew:
    mkdir -p build
    {{ cc }} {{ libmew-cflags }} -o ./build/mew.o -c mew/src/all.c
    ar r ./build/libmew.a ./build/mew.o

generate-comile-commands:
    #!/usr/bin/env sh

    mkdir -p build
    rm -f ./compile_commands.json
    for file in $(find mew -type f -name "*.c"); do
        bear --append -- {{ cc }} {{ libmew-cflags }} -o ./build/temp.o -c $file
    done
    for file in $(find website -type f -name "*.c"); do
        bear --append -- {{ cc }} {{ website-cflags }} -o ./build/temp.o -c $file
    done

test IMAGE:
    docker build -t n0emo-website .

serve: build
    ./build/server

watch:
    watchexec -r -w mew -w website -- "just profile={{ profile }} serve"

valgrind: build
    valgrind ./build/server

deploy:
    fly deploy
