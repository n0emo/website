set unstable

profile := "debug"

cc := require("clang")

docker := if which("docker") != "" {
    "docker"
} else if which("podman") != "" {
    "podman"
} else {
    "docker"
}

cflags := if profile == "debug" {
    "-Wall -Wextra -Isrc -Wno-unused-parameter -DLOG_WITH_FILE -g -Wunaligned-access -fsanitize=undefined -fsanitize=address"
} else if profile == "release" {
    "-Wall -Wextra -Isrc -Wno-unused-parameter -O2"
} else {
    ""
}

default: build

build:
    mkdir -p build
    {{ cc }} -std=gnu17 -o ./build/server {{ cflags }} src/server.c

generate-comile-commands:
    mkdir -p build
    bear -- {{ cc }} -std=gnu17 -o ./build/server {{ cflags }} `find src/ -type f -name "*.c" -not -path "src/server.c" -print0 | xargs -0`

test IMAGE:
    {{ docker }} build -t n0emo-website .

serve: build
    ./build/server

watch:
    watchexec -r -w src -- "just profile={{ profile }} serve"

valgrind: build
    valgrind ./build/server

deploy:
    fly deploy
