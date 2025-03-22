#include "mew/os/socket.h"

#include <fcntl.h>
#include <strings.h>
#include <errno.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifndef __APPLE__
#include <sys/sendfile.h>
#endif

#include "mew/log.h"
#include "mew/utils.h"

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
    bzero(options, sizeof(*options));
    options->reuse_address = true;
}

bool mew_tcplistener_init_native(MewTcpListener *listener, MewNativeTcpListenerOptions options) {
    bool result = true;

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        log_error("Error creating socker: %s", strerror(errno));
        return_defer(false);
    }

    if (options.reuse_address) {
        int option = 1;
        int ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (ret == -1) {
            log_error("Error creating socket: %s", strerror(errno));
            return_defer(false);
        }
    }

    listener->data = (void *) (uintptr_t) sd;
    listener->bind = &mew_tcplistener_native_bind;
    listener->listen = &mew_tcplistener_native_listen;
    listener->accept = &mew_tcplistener_native_accept;
    listener->close = &mew_tcplistener_native_close;
    return true;

defer:
    if (sd != -1) close(sd);
    return result;
}

bool mew_tcplistener_native_bind(void *data, const char *host, uint16_t port) {
    int ret;
    int sd = (int) (uintptr_t) data;

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_zero = { 0 },
    };

    ret = inet_pton(AF_INET, host, &addr.sin_addr);
    if (ret != 1) {
        log_error("Invalid address: %s", strerror(errno));
        return false;
    }

    ret = bind(sd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        log_error("Error binding socket: %s", strerror(errno));
        return false;
    }

    return true;
}

bool mew_tcplistener_native_listen(void *data, uint32_t max_connections) {
    int sd = (int) (uintptr_t) data;
    int ret = listen(sd, max_connections);
    if (ret == -1) {
        log_error("Error listening: %s", strerror(errno));
        return false;
    } else {
        return true;
    }
}

bool mew_tcplistener_native_accept(void *data, MewTcpStream *stream) {
    int sd = (int) (uintptr_t) data;
    int stream_sd = accept(sd, NULL, NULL);
    if (stream_sd == -1) return false;
    bzero(stream, sizeof(*stream));
    stream->data = (void *) (uintptr_t) stream_sd;
    stream->set_timeout = &mew_tcpstream_native_set_timeout;
    stream->read = &mew_tcpstream_native_read;
    stream->write = &mew_tcpstream_native_write;
    stream->sendfile = &mew_tcpstream_native_sendfile;
    stream->close = &mew_tcpstream_native_close;
    return true;
}

bool mew_tcplistener_native_close(void *data) {
    int sd = (int) (uintptr_t) data;
    if (sd == -1) return true;
    return (close(sd) == 0);
}

bool mew_tcpstream_native_set_timeout(void *data, uint32_t seconds) {
    int sd = (int) (uintptr_t) data;
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    int ret = setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    return ret != -1;
}

ptrdiff_t mew_tcpstream_native_read(void *data, char *buf, uintptr_t size) {
    int sd = (int) (uintptr_t) data;
    return recv(sd, buf, size, 0);
}

ptrdiff_t mew_tcpstream_native_write(void *data, const char *buf, uintptr_t size) {
    int sd = (int) (uintptr_t) data;
    return write(sd, buf, size);
}

bool mew_tcpstream_native_sendfile(void *data, const char *path, uintptr_t size) {
    log_info("Sendfile");
    int sd = (int) (uintptr_t) data;
    int body_fd = open(path, O_RDONLY);
    if (body_fd < 0) return false;

#ifdef __APPLE__
    off_t offset = (off_t) sf.size;
    int ret = sendfile(body_fd, sd, 0, &offset, NULL, 0);
    if (close(body_fd) < 0 || ret != 0) return false;
#else
    int ret = sendfile(sd, body_fd, NULL, size);
    if (close(body_fd) < 0 || ret != (ssize_t) size) return false;
#endif

    return true;
}

bool mew_tcpstream_native_close(void *data) {
    int sd = (int) (uintptr_t) data;
    return close(sd) == 0;
}
