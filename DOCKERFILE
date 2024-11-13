FROM clang:latest

WORKDIR /app

COPY src/ ./src/
RUN mkdir -p build
RUN clang -Wall -Wextra -DLOG_WITH_FILE src/*.c -g -o build/server

CMD ["./build/server"]
