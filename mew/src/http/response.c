#include "mew/http/response.h"

#include <assert.h>
#include <stdio.h>

#include "mew/http/common.h"
#include "mew/log.h"

bool http_response_write(HttpResponse *response, MewTcpStream stream) {
    if (!mew_tcpstream_write_cstr(stream, "HTTP/1.1 ")) return false;
    char status[5];
    snprintf(status, 5, "%d ", response->status);
    if (!mew_tcpstream_write_cstr(stream, status)) return false;
    if (!mew_tcpstream_write_cstr(stream, http_status_desc(response->status))) return false;
    if (!mew_tcpstream_write_cstr(stream, "\r\n")) return false;

    const HttpHeaderMapEntries *entries = &response->headers.entries;
    for (size_t i = 0; i < entries->count; i++) {
        const HttpHeader *h = (const HttpHeader *) &entries->items[i].header;

        if (!mew_tcpstream_write(stream, h->key.items, h->key.count)) return false;
        if (!mew_tcpstream_write_cstr(stream, ": ")) return false;
        if (!mew_tcpstream_write(stream, h->value.items, h->value.count)) return false;
        if (!mew_tcpstream_write_cstr(stream, "\r\n")) return false;
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

    char buf[32];
    snprintf(buf, 32, "%zu", content_length);
    if (!mew_tcpstream_write_cstr(stream, "Content-Length: ")) return false;
    if (!mew_tcpstream_write_cstr(stream, buf)) return false;
    if (!mew_tcpstream_write_cstr(stream, "\r\n\r\n")) return false;

    switch (response->body.kind) {
        case RESPONSE_BODY_NONE:
            log_debug("Response was none");
            break;
        case RESPONSE_BODY_BYTES: {
            StringBuilder *sb = &response->body.as.bytes;
            log_debug("Writing %zu bytes to response", sb->count);
            if (mew_tcpstream_write(stream, sb->items, sb->count) < 0) return false;
        } break;
        case RESPONSE_BODY_SENDFILE: {
            ResponseSendFile sf = response->body.as.sendfile;
            log_debug("Sending file %s with size %zu", sf.path, sf.size);
            mew_tcpstream_sendfile(stream, sf.path, sf.size);
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
