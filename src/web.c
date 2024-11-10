#include "web.h"

bool handle_request(Request *request, Response *response) {
    if (sv_eq_cstr(request->resource_path, "/")) {
        response->status = HTTP_OK;
        headers_insert_cstrs(&response->headers, "Content-Type", "text/html; charset=UTF-8");
        render_index(&response->body);
    } else {
        response->status = HTTP_NOT_FOUND;
    }

    return true;
}

void render_index(ArenaStringBuilder *sb) {
    sb_append_cstr(
        sb,
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
