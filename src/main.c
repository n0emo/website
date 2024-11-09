#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define INITIAL_CAP 4

#define ARRAY_APPEND(array, item) do { \
    if ((array)->items == NULL) { \
        (array)->items = malloc(INITIAL_CAP * sizeof((array)->items[0])); \
        (array)->count = 0; \
        (array)->capacity = INITIAL_CAP; \
    } else if ((array)->count >= (array)->capacity) { \
        size_t new_capacity = (array)->capacity * 2; \
        (array)->items = realloc((array)->items, new_capacity); \
        (array)->capacity = new_capacity; \
    } \
    (array)->items[(array)->count] = (item); \
    (array)->count++; \
} while(0) \

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} StringBuilder;

void sb_append_cstr(StringBuilder *sb, const char *s) {
    while(*s != '\0') {
        ARRAY_APPEND(sb, *s);
        s++;
    }
}

typedef struct {
    char *items;
    size_t count;
} StringView;

StringView cstr_to_sv(char *cstr) {
    return (StringView) {
        .items = cstr,
        .count = strlen(cstr),
    };
}

typedef struct {
    StringView key;
    StringView value;
} Header;

typedef struct {
    Header *items;
    size_t count;
    size_t capacity;
} HeaderMap;

void headers_insert(HeaderMap *map, Header header) {
    ARRAY_APPEND(map, header);
}

typedef struct {
    int status;
    HeaderMap headers;
    StringBuilder body;
} Response;

Response response_new() {
    Response response = {0};
    response.status = 200;
    headers_insert(
        &response.headers,
        (Header) {
            .key = cstr_to_sv("Connection"),
            .value = cstr_to_sv("close"),
        }
    );
    return response;
}

const char *status_desc(int status) {
    switch (status) {
        case 200:
            return "OK";
        default:
            return "UNKNOWN";
    }
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

void render_index(StringBuilder *sb) {
    sb_append_cstr(sb,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "   <head>\n"
        "       <title>An Example Page</title>\n"
        "   </head>\n"
        "   <body>\n"
        "       <h1>Hello World, this is a very simple HTML document.</h1>\n"
        "   </body>\n"
        "</html>"
    );
}

typedef struct {
    int connfd;
} ThreadData;

int handle_request(void *arg) {
    ThreadData *data = (ThreadData *) arg;

    Response response = response_new();
    render_index(&response.body);
    headers_insert(
        &response.headers,
        (Header) {
            .key = cstr_to_sv("Content-Type"),
            .value = cstr_to_sv("text/html; charset=UTF-8"),
        }
    );

    if (!write_response(data->connfd, response)) {
        perror("Error writing response");
    }

    close(data->connfd);

    free(response.body.items);
    free(data);

    return 0;
}

int main() {
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("error creating socket");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_addr = {
            .s_addr = htonl(INADDR_ANY),
        },
        .sin_family = AF_INET,
        .sin_port = htons(9001),
        .sin_zero = { 0 },
    };

    int result = bind(sd, (struct sockaddr *) &addr, sizeof(addr));
    if (result == -1) {
        perror("error binding");
        close(sd);
        return 1;
    }

    listen(sd, 100);

    char name[INET6_ADDRSTRLEN];
    char port[10];
    getnameinfo((struct sockaddr *) &addr, sizeof(addr), name, sizeof(name), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
    printf("Serving at %s:%s\n", name, port);
    while (true) {
        int connfd = accept(sd, NULL, NULL);
        thrd_t thrd;
        ThreadData *data = malloc(sizeof(ThreadData));
        data->connfd = connfd;
        if (thrd_create(&thrd, handle_request, (void *) data) != thrd_success) {
            perror("error creating thread");
            free(data);
            close(connfd);
        }
    }

    return 0;
}
