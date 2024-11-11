#include "http.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include "utils.h"

bool start_server(int port, request_handler_t *handler) {
    bool result = true;

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("error creating socket");
        return_defer(false);
    }

    struct sockaddr_in addr = {
        .sin_addr = {
            .s_addr = htonl(INADDR_ANY),
        },
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_zero = { 0 },
    };

    int option = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    int ret = bind(sd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        perror("error binding");
        return_defer(false);
    }

    ret = listen(sd, 100);
    if (ret == -1) {
        perror("error listening");
        return_defer(false);
    }

    char addr_name[INET6_ADDRSTRLEN];
    char addr_port[10];
    getnameinfo((struct sockaddr *) &addr, sizeof(addr), addr_name, sizeof(addr_name), addr_port, sizeof(addr_port), NI_NUMERICHOST | NI_NUMERICSERV);
    printf("Serving at %s:%s\n", addr_name, addr_port);

    while (true) {
        accept_connection(sd, handler);
    }

defer:
    if (sd != -1) close(sd);
    return result;
}

bool accept_connection(int sd, request_handler_t *handler) {
    bool result = true;
    ThreadData *data = NULL;
    int connfd = accept(sd, NULL, NULL);

    if (connfd == -1) {
        perror("error accepting connection");
        return_defer(false);
    }

    pthread_t thrd;
    data = calloc(1, sizeof(ThreadData));
    data->connfd = connfd;
    data->handler = handler;
    // TODO: maybe we can preallocate threads and reuse themto handle requests
    if (pthread_create(&thrd, NULL, (void *(*)(void *)) handle_connection, (void *) data) != 0) {
        perror("error creating thread");
        return_defer(false);
    }

defer:
    if (!result) {
        if (connfd != -1) close(connfd);
        if (data) free(data);
    }
    return result;
}

bool handle_connection(ThreadData *data) {
    pthread_detach(pthread_self());

    bool result = true;
    Request request = {0};
    Response response = {0};
    request.headers.arena = &request.arena;
    response.body.arena = &request.arena;
    response.headers.arena = &request.arena;

    try(parse_request(data->connfd, &request));
    headers_insert_cstrs(&response.headers, "Connection", "close");
    try(data->handler(&request, &response));
    try(write_response(data->connfd, response));

defer:
    arena_free(&request.arena);
    close(data->connfd);
    free(data);
    return result;
}

// TODO: more statuses
const char *status_desc(HttpStatus status) {
    switch (status) {
        case HTTP_OK:                    return "OK";
        case HTTP_NOT_FOUND:             return "NOT FOUND";
        case HTTP_INTERNAL_SERVER_ERROR: return "INTERNAL SERVER ERROR";

        default: return "UNKNOWN";
    }
}

// TODO: hash map or binary tree
void headers_insert(HeaderMap *map, Header header) {
    ARRAY_APPEND_ARENA(map, header, map->arena);
}

void headers_insert_cstrs(HeaderMap *map, const char *key, const char *value) {
    Header header = {
        .key = cstr_to_sv(key),
        .value = cstr_to_sv(value),
    };

    headers_insert(map, header);
}

bool write_response(int fd, Response response) {
    if (dprintf(fd, "HTTP/1.1 %d %s\n", response.status, status_desc(response.status)) < 0) {
        return false;
    }

    for (size_t i = 0; i < response.headers.count; i++) {
        Header h = response.headers.items[i];
        int ret = dprintf(fd,
            "%.*s: %.*s\n",
            (int) h.key.count, h.key.items,
            (int) h.value.count, h.value.items
        );

        if (ret < 0) {
            return false;
        }
    }

    if (dprintf(fd, "Content-Length: %zu\n\n", response.body.count) < 0) {
        return false;
    }

    if (dprintf(fd, "%.*s", (int) response.body.count, response.body.items) < 0) {
        return false;
    }

    return true;
}

// TODO: streaming response

#define BUF_CAP 8192

bool request_peek(int sd, StringView *sv, char buf[BUF_CAP]) {
    ssize_t bytes = recv(sd, buf, BUF_CAP, MSG_PEEK | MSG_DONTWAIT);
    if (bytes < 0) return false;
    sv->items = buf;
    sv->count = bytes;
    return true;
}

bool request_advance(int sd, StringView *sv, char buf[BUF_CAP], size_t count) {
    if (recv(sd, buf, count, 0) != (ssize_t) count) return false;
    return request_peek(sd, sv, buf);
}

// TODO: general trim in sv
void request_trim_cr(StringView *sv) {
    if (sv->items[sv->count - 1] == '\r') sv->count--;
}

// TODO: arena memdup
StringView request_copy_sv(Arena *arena, StringView sv) {
    char *items = arena_alloc(arena, sv.count);
    assert(items != NULL);
    memcpy(items, sv.items, sv.count);
    return (StringView) { items, sv.count };
}

bool parse_request(int fd, Request *request) {
    // TODO maybe it's not a good idea to allocate 8Kb at the stack for each request
    char buf[BUF_CAP] = {0};
    StringView sv = {0};

    if (!request_peek(fd, &sv, buf)) return false;

    StringView status_line = sv_chop_by(&sv, '\n');
    size_t count = status_line.count + 1;
    request_trim_cr(&status_line);

    StringView method = sv_chop_by(&status_line, ' ');

    if (sv_eq_cstr(method, "GET")) {
        request->method = HTTP_GET;
    } else if (sv_eq_cstr(method, "POST")) {
        request->method = HTTP_POST;
    } else {
        return false;
    }

    request->resource_path = request_copy_sv(&request->arena, sv_chop_by(&status_line, ' '));
    request->version = request_copy_sv(&request->arena, sv_chop_by(&status_line, '\n'));

    if (!request_advance(fd, &sv, buf, count)) return false;

    StringView header_line;
    while (true) {
        header_line = sv_chop_by(&sv, '\n');
        size_t count = header_line.count + 1;
        request_trim_cr(&header_line);

        if (header_line.count == 0) {
            // TODO: if the last line is just \n, not \r\n, first byte of the body will be lost
            recv(fd, buf, 2, 0);
            break;
        }

        StringView key = request_copy_sv(&request->arena, sv_chop_by(&header_line, ':'));
        StringView value = request_copy_sv(&request->arena, sv_slice_from(header_line, 1));
        headers_insert(&request->headers, (Header) { key, value });

        if (!request_advance(fd, &sv, buf, count)) return false;
    }

    StringView resource_path = request->resource_path;
    ArenaStringBuilder sb = { .arena = &request->arena, 0 };
    StringView path = sv_chop_by(&resource_path, '?');
    if (!http_urldecode(path, &sb)) return false;
    request->path = sb_to_sv(sb);

    request->query_string = resource_path;

    return true;
}

bool http_urldecode(StringView sv, ArenaStringBuilder *out) {
    static const char *allowed = "!#$&'()*+,/:;=?@[]ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";

    while (sv.count > 0) {
        char c = sv.items[0];
        if (strchr(allowed, c)) {
            sb_append_char(out, c);
            sv = sv_slice_from(sv, 1);
        } else if (c == '%') {
            if (sv.count < 3) return false;
            char digits[3] = {0};
            digits[0] = sv.items[1];
            digits[1] = sv.items[2];
            char result = (char) strtol(digits, NULL, 16);
            if (result == 0) return false;
            sb_append_char(out, result);
            sv = sv_slice_from(sv, 3);
        } else {
            return false;
        }
    }

    return true;
}

