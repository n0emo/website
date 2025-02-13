#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "http/http.h"
#include "web.h"
#include "log.h"

int main() {
    int port = 9000;
    const char *port_env = getenv("SERVER_PORT");

    if (port_env != NULL) {
        int new_port = atoi(port_env);
        if (new_port != 0) {
            port = new_port;
        }
    }

    Arena arena = {0};
    HttpRouter router = {0};
    http_router_init(&router, NULL, new_arena_allocator(&arena));
    web_setup_handlers(&router);

    HttpServer server;
    http_server_init(&server, router, (HttpServerSettings) {
        .port = port,
    });

    if (!http_server_start(&server)) {
        log_error("Error starting server: %s", strerror(errno));
        return 1;
    }

    return 0;
}

