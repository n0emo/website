FROM silkeh/clang:latest

WORKDIR /app

COPY src/ ./src/
COPY assets/ ./assets/
COPY blogs/ ./blogs/
RUN mkdir -p build
RUN clang -Wall -Wextra -DLOG_WITH_FILE src/*.c -g -o build/server
ENV SERVER_PORT=8080

CMD ["./build/server"]
