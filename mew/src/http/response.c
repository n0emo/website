#include "mew/http/response.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __APPLE__
#include <sys/socket.h>
#else
#include <sys/sendfile.h>
#endif

#include "mew/http/common.h"
#include "mew/log.h"

bool http_response_write(HttpResponse *response, int fd) {
    if (dprintf(fd, "HTTP/1.1 %d %s\r\n", response->status, http_status_desc(response->status)) < 0) {
        return false;
    }

    const HttpHeaderMapEntries *entries = &response->headers.entries;
    for (size_t i = 0; i < entries->count; i++) {
        const HttpHeader *h = (const HttpHeader *) &entries->items[i].header;

        int ret = dprintf(
            response->sd,
            SV_FMT ": " SV_FMT "\r\n",
            SV_ARG(h->key), SV_ARG(h->value)
        );

        if (ret < 0) {
            return false;
        }

    }

    size_t content_length = 0;
    switch (response->body.kind) {
        case RESPONSE_BODY_NONE:
            break;
        case RESPONSE_BODY_BYTES:
            content_length = response->body.as.bytes.count;
            break;
        case RESPONSE_BODY_SENDFILE:
            content_length = response->body.as.sendfile.size;
            break;
    }

    if (content_length > 0) {
        if (dprintf(fd, "Content-Length: %zu\r\n\r\n", content_length) < 0) {
            return false;
        }
    }

    switch (response->body.kind) {
        case RESPONSE_BODY_NONE:
            break;
        case RESPONSE_BODY_BYTES: {
            StringBuilder *sb = &response->body.as.bytes;
            log_debug("Writing %zu bytes to response", sb->count);
            if (write(fd, sb->items, sb->count) < 0) return false;
        } break;
        case RESPONSE_BODY_SENDFILE: {
            ResponseSendFile sf = response->body.as.sendfile;
            log_debug("Sending file %s with size %zu", sf.path, sf.size);

            int body_fd = open(sf.path, O_RDONLY);
            if (body_fd < 0) return false;

#ifdef __APPLE__
            off_t offset = (off_t) sf.size;
            int ret = sendfile(body_fd, fd, 0, &offset, NULL, 0);
            if (close(body_fd) < 0 || ret != 0) return false;
#else
            int ret = sendfile(fd, body_fd, NULL, sf.size);
            if (close(body_fd) < 0 || ret != (ssize_t) sf.size) return false;
#endif
        };
      break;
    }

    return true;
}

void http_response_body_set_bytes(HttpResponse *response) {
    assert(response->body.kind == RESPONSE_BODY_NONE);
    response->body.kind = RESPONSE_BODY_BYTES;
    response->body.as.bytes = (StringBuilder) {0};
    response->body.as.bytes.alloc = response->body.alloc;
}

void http_response_body_set_sendfile(HttpResponse *response, ResponseSendFile sendfile) {
    assert(response->body.kind == RESPONSE_BODY_NONE);
    response->body.kind = RESPONSE_BODY_SENDFILE;
    response->body.as.sendfile = sendfile;
}

void http_response_set_html(HttpResponse *response) {
    assert(response->body.kind == RESPONSE_BODY_BYTES);
    assert(response->body.as.bytes.count > 0);
    response->status = HTTP_OK;
    http_headermap_insert_cstrs(&response->headers, "Content-Type", "text/html; charset=UTF-8");
}

void http_response_not_found(HttpResponse *response) {
    response->status = HTTP_NOT_FOUND;
}

void http_response_set_internal_server_error(HttpResponse *response) {
    response->status = HTTP_INTERNAL_SERVER_ERROR;
}
