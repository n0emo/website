# Stage 1: Build
FROM silkeh/clang:latest AS builder

WORKDIR /

COPY ./src/ ./src/
COPY ./build.sh ./build.sh

RUN chmod +x ./build.sh
RUN ./build.sh

# Stage 2: Final image
FROM debian:latest

WORKDIR /app

COPY ./assets/ /app/assets/
COPY ./blogs/ /app/blogs/
COPY --from=builder /build/server /app/server

ENV SERVER_PORT=8080

CMD ["/app/server"]
