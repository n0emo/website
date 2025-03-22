#ifndef MEW_INCLUDE_MEW_OS_SOCKET_H_
#define MEW_INCLUDE_MEW_OS_SOCKET_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct MewTcpListener MewTcpListener;
typedef struct MewTcpStream MewTcpStream;

/********************************** TcpListener ************************************/

typedef bool (mew_tcplistener_bind_t)(void *data, const char *host, uint16_t port);
typedef bool (mew_tcplistener_listen_t)(void *data, uint32_t max_connections);
typedef bool (mew_tcplistener_accept_t)(void *data, MewTcpStream *stream);
typedef bool (mew_tcplistener_close_t)(void *data);

struct MewTcpListener {
    void *data;

    mew_tcplistener_bind_t *bind;
    mew_tcplistener_listen_t *listen;
    mew_tcplistener_accept_t *accept;
    mew_tcplistener_close_t *close;
};

static inline bool mew_tcplistener_bind(MewTcpListener listener, const char *host, uint16_t port) {
    return listener.bind(listener.data, host, port);
}

static inline bool mew_tcplistener_listen(MewTcpListener listener, uint32_t max_connections) {
    return listener.listen(listener.data, max_connections);
}

static inline bool mew_tcplistener_accept(MewTcpListener listener, MewTcpStream *stream) {
    return listener.accept(listener.data, stream);
}

static inline bool mew_tcplistener_close(MewTcpListener listener) {
    return listener.close(listener.data);
}

typedef struct MewNativeTcpListenerOptions {
    uint16_t port;
    bool reuse_address;
} MewNativeTcpListenerOptions;

void mew_tcplistener_init_default_native_options(MewNativeTcpListenerOptions *options);
bool mew_tcplistener_init_native(MewTcpListener *sock, MewNativeTcpListenerOptions options);


/**********************************  TcpStream  ************************************/

typedef bool (mew_tcpstream_set_timeout_t)(void *data, uint32_t seconds);
typedef ptrdiff_t (mew_tcpstream_read_t)(void *data, char *buf, uintptr_t size);
typedef ptrdiff_t (mew_tcpstream_write_t)(void *data, const char *buf, uintptr_t size);
typedef bool (mew_tcpstream_sendfile_t)(void *data, const char *path, uintptr_t size);
typedef bool (mew_tcpstream_close_t)(void *data);

struct MewTcpStream {
    void *data;

    mew_tcpstream_set_timeout_t *set_timeout;
    mew_tcpstream_read_t *read;
    mew_tcpstream_write_t *write;
    mew_tcpstream_sendfile_t *sendfile;
    mew_tcpstream_close_t *close;
};


static inline bool mew_tcpstream_set_timeout(MewTcpStream stream, uint32_t seconds) {
    return stream.set_timeout(stream.data, seconds);
}

static inline ptrdiff_t mew_tcpstream_read(MewTcpStream stream, char *buf, uintptr_t size) {
    return stream.read(stream.data, buf, size);
}

static inline ptrdiff_t mew_tcpstream_write(MewTcpStream stream, const char *buf, uintptr_t size) {
    return stream.write(stream.data, buf, size);
}

static inline bool mew_tcpstream_sendfile(MewTcpStream stream, const char *path, uintptr_t size) {
    return stream.sendfile(stream.data, path, size);
}

static inline bool mew_tcpstream_close(MewTcpStream stream) {
    return stream.close(stream.data);
}

static inline bool mew_tcpstream_write_cstr(MewTcpStream stream, const char *cstr) {
    return stream.write(stream.data, cstr, strlen(cstr));
}

// TODO: SSL Socket

#endif // MEW_INCLUDE_MEW_OS_SOCKET_H_
