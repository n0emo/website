#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <netinet/in.h>
#include <stdint.h>

#include "mew/http/router.h"
#include "mew/thrdpool.h"

typedef struct HttpServerSettings {
    uint16_t port;
} HttpServerSettings;

typedef struct HttpServer {
    ThreadPool thread_pool;
    HttpRouter router;
    int socket;
    HttpServerSettings settings;
    struct sockaddr_in address;
} HttpServer;

bool http_server_init(HttpServer *server, HttpRouter router, HttpServerSettings settings);
void http_server_destroy(HttpServer *server);
bool http_server_start(HttpServer *server);

#endif // HTTP_SERVER_H_
