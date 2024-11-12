#include <stdio.h>
#include <stdlib.h>

#include "http.h"
#include "web.h"

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
        // TODO: better logging system
        fprintf(stderr, "Error starting server");
        return 1;
    }

    return 0;
}

