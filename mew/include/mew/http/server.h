#ifndef MEW_INCLUDE_MEW_HTTP_SERVER_H_
#define MEW_INCLUDE_MEW_HTTP_SERVER_H_

#include <netinet/in.h>
#include <stdint.h>

#include "mew/http/router.h"
#include "mew/os/socket.h"
#include "mew/thrdpool.h"

typedef struct HttpServerSettings {
    uint16_t port;
    const char *host;
} HttpServerSettings;

typedef struct HttpServer {
    ThreadPool thread_pool;
    HttpRouter router;
    MewTcpListener listener;
    HttpServerSettings settings;
} HttpServer;

bool http_server_init(HttpServer *server, HttpRouter router, HttpServerSettings settings);
void http_server_destroy(HttpServer *server);
bool http_server_start(HttpServer *server);

#endif // MEW_INCLUDE_MEW_HTTP_SERVER_H_
