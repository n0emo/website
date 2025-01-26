# Stage 1: Build
FROM ubuntu:latest AS builder

WORKDIR /

COPY ./src/ ./src/
COPY ./justfile justfile

RUN apt update && apt install clang just -y
RUN just profile=release build

# Stage 2: Final image
FROM ubuntu:latest

WORKDIR /app

COPY ./assets/ /app/assets/
COPY ./blogs/ /app/blogs/
COPY --from=builder /build/server /app/server

ENV SERVER_PORT=8080

CMD ["/app/server"]
