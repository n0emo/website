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
    "-Wall -Wextra -Isrc -g -fsanitize=undefined -fsanitize=address"
} else if profile == "release" {
    "-Wall -Wextra -Isrc -O2"
} else {
    ""
}

default: build

build:
    mkdir -p build
    {{ cc }} -o ./build/server {{ cflags }} src/server.c

test IMAGE:
    {{ docker }} build -t n0emo-website .

serve: build
    ./build/server

watch:
    watchexec -r -w src -- "just profile={{ profile }} serve"

deploy:
    fly deploy
