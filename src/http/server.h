#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <netinet/in.h>
#include <stdint.h>

#include "http/request.h"
#include "http/response.h"
#include "thrdpool.h"

typedef bool http_request_handler_t(HttpRequest *request, HttpResponse *response);

typedef struct HttpServerSettings {
    uint16_t port;
} HttpServerSettings;

typedef struct HttpServer {
    ThreadPool thread_pool;
    int socket;
    http_request_handler_t *handler;
    HttpServerSettings settings;
    struct sockaddr_in address;
} HttpServer;

bool http_server_init(HttpServer *server, http_request_handler_t *handler, HttpServerSettings settings);
void http_server_destroy(HttpServer *server);
bool http_server_start(HttpServer *server);

#endif // HTTP_SERVER_H_
