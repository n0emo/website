#include "mew/os/socket.h"

#include <windows.h>

mew_tcplistener_bind_t mew_tcplistener_native_bind;
mew_tcplistener_listen_t mew_tcplistener_native_listen;
mew_tcplistener_accept_t mew_tcplistener_native_accept;
mew_tcplistener_close_t mew_tcplistener_native_close;

mew_tcpstream_set_timeout_t mew_tcpstream_native_set_timeout;
mew_tcpstream_read_t mew_tcpstream_native_read;
mew_tcpstream_write_t mew_tcpstream_native_write;
mew_tcpstream_sendfile_t mew_tcpstream_native_sendfile;
mew_tcpstream_close_t mew_tcpstream_native_close;

void mew_tcplistener_init_default_native_options(MewNativeTcpListenerOptions *options) {
    #error "mew_tcplistener_init_default_native_options is not implemented on Windows"
}

bool mew_tcplistener_init_native(MewTcpListener *listener, MewNativeTcpListenerOptions options) {
    #error "mew_tcplistener_init_native is not implemented on Windows"
}

bool mew_tcplistener_native_bind(void *data, const char *host, uint16_t port) {
    #error "mew_tcplistener_native_bind is not implemented on Windows"
}

bool mew_tcplistener_native_listen(void *data, uint32_t max_connections) {
    #error "mew_tcplistener_native_listen is not implemented on Windows"
}

bool mew_tcplistener_native_accept(void *data, MewTcpStream *stream) {
    #error "mew_tcplistener_native_accept is not implemented on Windows"
}

bool mew_tcplistener_native_close(void *data) {
    #error "mew_tcplistener_native_close is not implemented on Windows"
}

bool mew_tcpstream_native_set_timeout(void *data, uint32_t seconds) {
    #error "mew_tcpstream_native_set_timeout is not implemented on Windows"
}

ptrdiff_t mew_tcpstream_native_read(void *data, char *buf, uintptr_t size) {
    #error "mew_tcpstream_native_read is not implemented on Windows"
}

ptrdiff_t mew_tcpstream_native_write(void *data, const char *buf, uintptr_t size) {
    #error "mew_tcpstream_native_write is not implemented on Windows"
}

bool mew_tcpstream_native_sendfile(void *data, const char *path, uintptr_t size) {
    #error "mew_tcpstream_native_sendfile is not implemented on Windows"
}

bool mew_tcpstream_native_close(void *data) {
    #error "mew_tcpstream_native_close is not implemented on Windows"
}
