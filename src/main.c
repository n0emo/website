#include <stdio.h>

#include "http.h"
#include "web.h"

int main() {
    // TODO: get port from env
    if (!start_server(9000, &handle_request)) {
        // TODO: better logging system
        fprintf(stderr, "Error starting server");
        return 1;
    }

    return 0;
}

