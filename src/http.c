#include "http.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include "log.h"
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
    log_info("Serving at %s:%s", addr_name, addr_port);

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
    request.alloc = new_arena_allocator(&request.arena);
    request.headers.alloc = &request.alloc;
    request.body.alloc = &request.alloc;
    request.sd = data->connfd;

    Response response = {0};
    response.body.arena = &request.arena;
    response.body.alloc = &request.alloc;
    response.headers.alloc = &request.alloc;
    response.sd = data->connfd;

    try(parse_request(data->connfd, &request));
    headers_insert_cstrs(&response.headers, "Connection", "close");
    headers_insert_cstrs(&response.headers, "X-Frame-Options", "DENY");
    headers_insert_cstrs(&response.headers, "Content-Security-Policy", "default-src 'self';");
    try(data->handler(&request, &response));
    try(write_response(data->connfd, response));

defer:
    arena_free_arena(&request.arena);
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

const char *method_str(HttpMethod method) {
    switch (method) {
        case HTTP_GET:  return "GET";
        case HTTP_POST: return "POST";
    }

    return "UNKNOWN";
}

// TODO: hash map or binary tree
void headers_insert(HeaderMap *map, Header header) {
    ARRAY_APPEND(map, header, map->alloc);
}

void headers_insert_cstrs(HeaderMap *map, const char *key, const char *value) {
    Header header = {
        .key = cstr_to_sv(key),
        .value = cstr_to_sv(value),
    };

    headers_insert(map, header);
}

bool write_response(int sd, Response response) {
    if (dprintf(sd, "HTTP/1.1 %d %s\r\n", response.status, status_desc(response.status)) < 0) {
        return false;
    }

    for (size_t i = 0; i < response.headers.count; i++) {
        Header h = response.headers.items[i];
        int ret = dprintf(sd,
            SV_FMT ": " SV_FMT "\r\n",
            SV_ARG(h.key), SV_ARG(h.value)
        );

        if (ret < 0) {
            return false;
        }
    }

    size_t content_length = 0;
    switch (response.body.kind) {
        case RESPONSE_BODY_NONE:
            break;
        case RESPONSE_BODY_BYTES:
            content_length = response.body.as.bytes.count;
            break;
        case RESPONSE_BODY_SENDFILE:
            content_length = response.body.as.sendfile.size;
            break;
    }

    if (content_length > 0) {
        if (dprintf(sd, "Content-Length: %zu\r\n\r\n", content_length) < 0) {
            return false;
        }
    }

    switch (response.body.kind) {
        case RESPONSE_BODY_NONE:
            break;
        case RESPONSE_BODY_BYTES: {
            StringBuilder *sb = &response.body.as.bytes;
            log_debug("Writing %zu bytes to response", sb->count);
            if (write(sd, sb->items, sb->count) < 0) return false;
        } break;
        case RESPONSE_BODY_SENDFILE: {
            ResponseSendFile sf = response.body.as.sendfile;
            log_debug("Sending file %s with size %zu", sf.path, sf.size);

            int fd = open(sf.path, O_RDONLY);
            if (fd < 0) return false;

            int ret = sendfile(sd, fd, NULL, sf.size);
            if (close(fd) < 0 || ret != (ssize_t) sf.size) return false;
        };
      break;
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

void request_trim_cr(StringView *sv) {
    if (sv->items[sv->count - 1] == '\r') sv->count--;
}

// TODO: time limit
bool read_request_header_lines(int sd, StringBuilder *header, StringBuilder *body) {
    // TODO maybe it's not a good idea to allocate 8Kb at the stack for each request
    char buf[BUF_CAP] = {0};

    size_t count = 0;

    while (true) {
        ssize_t bytes = recv(sd, buf, BUF_CAP, 0);
        if (bytes < 0) {
            return false;
        }

        sb_append_buf(header, buf, bytes);
        sb_append_char(header, '\0');
        header->count--;

        const char *body_ptr = strstr(header->items + count, "\r\n\r\n");
        count = header->count;
        size_t newlines = 4;

        if (body_ptr == NULL) {
            body_ptr = strstr(header->items, "\n\n");
            newlines = 2;
        }

        if (body_ptr != NULL) {
            size_t header_size = (size_t) (body_ptr - header->items);
            sb_append_buf(body, body_ptr + newlines, header->count - header_size - newlines);
            header->count = header_size;
            return true;
        }
    }
}

bool parse_request(int fd, Request *request) {
    StringBuilder header = { .alloc = &request->alloc, 0 };

    if (!read_request_header_lines(fd, &header, &request->body)) return false;
    StringView sv = {
        .items = header.items,
        .count = header.count,
    };

    StringView status_line = sv_chop_by(&sv, '\n');
    request_trim_cr(&status_line);

    StringView method = sv_chop_by(&status_line, ' ');

    if (sv_eq_cstr(method, "GET")) {
        request->method = HTTP_GET;
    } else if (sv_eq_cstr(method, "POST")) {
        request->method = HTTP_POST;
    } else {
        return false;
    }

    request->resource_path = sv_dup(&request->alloc, sv_chop_by(&status_line, ' '));
    request->version = sv_dup(&request->alloc, sv_chop_by(&status_line, '\n'));

    StringView header_line;
    while (true) {
        header_line = sv_chop_by(&sv, '\n');
        request_trim_cr(&header_line);

        if (header_line.count == 0) {
            break;
        }

        StringView key = sv_dup(&request->alloc, sv_chop_by(&header_line, ':'));
        StringView value = sv_dup(&request->alloc, sv_slice_from(header_line, 1));
        headers_insert(&request->headers, (Header) { key, value });
    }

    StringView resource_path = request->resource_path;
    StringBuilder sb = { .alloc = &request->alloc, 0 };
    StringView path = sv_chop_by(&resource_path, '?');
    if (!http_urldecode(path, &sb)) return false;
    request->path = sb_to_sv(sb);

    request->query_string = resource_path;

    return true;
}

bool http_urldecode(StringView sv, StringBuilder *out) {
    static const char *allowed =
        "!#$&'()*+,/:;=?@[]"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789-._~";

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

bool try_serve_dir(Response *response, StringView file, StringView dir) {
    if (file.items[0] == '/') file = sv_slice_from(file, 1);

    char *path = mem_sprintf(
        response->body.alloc,
        SV_FMT "/" SV_FMT,
        SV_ARG(dir), SV_ARG(file)
    );

    if (strncmp(path, "../", 3) == 0 ||
        strncmp(path + strlen(path) - 3, "/..", 3) == 0 ||
        strstr(path, "/../") != NULL) {
        return false;
    }

    struct stat file_stat = {0};
    if (stat(path, &file_stat) != 0) return false;

    response->body.kind = RESPONSE_BODY_SENDFILE;
    response->body.as.sendfile = (ResponseSendFile) {
        .path = path,
        .size = file_stat.st_size,
    };

    return true;
}

