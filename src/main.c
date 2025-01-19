#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "http.h"
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

    if (!start_server(port, &handle_request)) {
        log_error("Error starting server: %s", strerror(errno));
        return 1;
    }

    return 0;
}

