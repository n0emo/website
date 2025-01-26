profile := "debug"
cflags := if profile == "debug" {
    "-Wall -Wextra -Isrc -g -fsanitize=undefined -fsanitize=address"
} else if profile == "release" {
    "-Wall -Wextra -Isrc -O2"
} else {
    ""
}

sources := `echo $(find src -name "*.c")`

default: build

build-dir:
    mkdir -p build

build: build-dir
    clang -o ./build/server {{ cflags }} {{sources}}

serve: build
    ./build/server

watch:
    watchexec -r -w src -- "just serve"

deploy:
    fly deploy
