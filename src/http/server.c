#include "http/server.h"

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.h"
#include "utils.h"

typedef struct {
    int connfd;
    HttpServer *server;
} ThreadData;

bool init_socket(HttpServer *server);
bool accept_connection(HttpServer *server);
int handle_connection(void *arg);

bool http_server_init(HttpServer *server, http_request_handler_t *handler, HttpServerSettings settings) {
    bool result;

    server->handler = handler;
    server->settings = settings;
    thrdpool_init(&server->thread_pool, 100);
    try(init_socket(server));

    return true;

defer:
    return result;
}

bool init_socket(HttpServer *server) {
    bool result = true;

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        log_error("Error creating socker: %s", strerror(errno));
        return_defer(false);
    }

    struct sockaddr_in addr = {
        .sin_addr = {
            .s_addr = htonl(INADDR_ANY),
        },
        .sin_family = AF_INET,
        .sin_port = htons(server->settings.port),
        .sin_zero = { 0 },
    };

    int option = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    int ret = bind(sd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        log_error("Error binding socket: %s", strerror(errno));
        return_defer(false);
    }

    server->socket = sd;
    server->address = addr;
    return true;

defer:
    if (sd != -1) close(sd);
    return result;
}

void http_server_destroy(HttpServer *server) {
    close(server->socket);
    thrdpool_destroy(&server->thread_pool);
}

bool http_server_start(HttpServer *server) {
    if (listen(server->socket, 100) == -1) {
        log_error("Error listening: %s", strerror(errno));
        return false;
    }

    char addr_name[INET6_ADDRSTRLEN];
    char addr_port[10];
    getnameinfo((struct sockaddr *) &server->address, sizeof(server->address), addr_name, sizeof(addr_name), addr_port, sizeof(addr_port), NI_NUMERICHOST | NI_NUMERICSERV);
    log_info("Serving at %s:%s", addr_name, addr_port);


    while (true) {
        accept_connection(server);
    }
}

bool accept_connection(HttpServer *server) {
    bool result = true;
    ThreadData *data = NULL;
    int connfd = accept(server->socket, NULL, NULL);

    if (connfd == -1) {
        log_error("Error accepting connection: %s", strerror(errno));
        return_defer(false);
    }

    data = calloc(1, sizeof(ThreadData));
    data->connfd = connfd;
    data->server = server;

    thrdpool_add_job(&server->thread_pool, handle_connection, (void *) data);

defer:
    if (!result) {
        if (connfd != -1) close(connfd);
        if (data) free(data);
    }
    return result;
}

int handle_connection(void *arg) {
    ThreadData *data = (ThreadData *) arg;

    bool result = true;
    HttpRequest request = {0};
    request.alloc = new_arena_allocator(&request.arena);
    http_headermap_init(&request.headers, request.alloc);
    request.body.alloc = &request.alloc;
    request.sd = data->connfd;

    HttpResponse response = {0};
    response.body.arena = &request.arena;
    response.body.alloc = &request.alloc;
    http_headermap_init(&response.headers, request.alloc);
    response.sd = data->connfd;

    try(http_request_parse(&request, data->connfd));
    http_headermap_insert_cstrs(&response.headers, "Connection", "close");
    http_headermap_insert_cstrs(&response.headers, "X-Frame-Options", "DENY");
    http_headermap_insert_cstrs(&response.headers, "Content-Security-Policy", "default-src 'self';");
    try(data->server->handler(&request, &response));
    try(http_response_write(&response, data->connfd));

defer:
    arena_free_arena(&request.arena);
    close(data->connfd);
    free(data);
    return !result;
}
