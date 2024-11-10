#include "web.h"

#include "html.h"

// TODO: serve file
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
    Html html = {0};

    html_begin(&html);

    html_head_begin(&html);
    html_title(&html, cstr_to_sv("_n0emo website"));
    html_head_end(&html);

    html_body_begin(&html);
    html_h1_begin(&html);
    html_text(&html, cstr_to_sv("Welcome to the _n0emo's personal website!"));
    html_h1_end(&html);
    html_body_end(&html);

    html_end(&html);

    html_render_to_sb_and_free(&html, sb);
}
