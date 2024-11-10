#include <stdio.h>

#include "http.h"
#include "web.h"

int main() {
    if (!start_server(9000, &handle_request)) {
        fprintf(stderr, "Error starting server");
        return 1;
    }

    return 0;
}

