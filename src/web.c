#include "web.h"

#include "html.h"
#include "utils.h"

// TODO: serve file
bool handle_request(Request *request, Response *response) {
    if (sv_eq_cstr(request->path, "/")) {
        response->status = HTTP_OK;
        headers_insert_cstrs(&response->headers, "Content-Type", "text/html; charset=UTF-8");
        render_index(&response->body);
    } else if (sv_eq_cstr(request->path, "/blogs")) {
        response->status = HTTP_OK;
        headers_insert_cstrs(&response->headers, "Content-Type", "text/html; charset=UTF-8");
        render_blogs(&response->body);
    } else if (file_exists_in_dir(request->path, cstr_to_sv("assets"))) {
        response->status = HTTP_INTERNAL_SERVER_ERROR;
    } else {
        response->status = HTTP_NOT_FOUND;
    }

    return true;
}

void render_index(ArenaStringBuilder *sb) {
    Html html = {0};
    page_base_begin(&html, "_n0emo website");

    html_h1_begin(&html);
    html_text(&html, "Welcome to the _n0emo's personal website!");
    html_h1_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

void render_blogs(ArenaStringBuilder *sb) {
    Html html = {0};
    page_base_begin(&html, "Blogs");

    html_h1_begin(&html);
    html_text(&html, "Recent blog posts:");
    html_h1_end(&html);

    html_ul_begin(&html);
    for (size_t i = 10; i < 20; i++) {
        html_li_begin(&html);
        html_text(&html, arena_sprintf(&html.arena, "Blog %zu", i));
        html_li_end(&html);
    }
    html_ul_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

void link_item(Html *html, const char *title, const char *destination) {
    html_li_begin(html);
    html_push_attribute_cstrs(html, "href", destination);
    html_a_begin(html);
    html_pop_attributes(html, 1);
    html_text(html, title);
    html_a_end(html);
    html_li_end(html);
}

void navigation_menu(Html *html) {
    html_nav_begin(html);
    html_h1_begin(html);
    html_text(html, "Navigation");
    html_h1_end(html);
    html_ul_begin(html);
    link_item(html, "Home", "/");
    link_item(html, "Blogs", "/blogs");
    html_ul_end(html);
    html_nav_end(html);
}

void page_base_begin(Html *html, char *title) {
    html_begin(html);

    html_head_begin(html);
    html_title(html, title);
    html_head_end(html);

    html_body_begin(html);
    navigation_menu(html);
    html_div_begin(html);
}

void page_base_end(Html *html) {
    html_div_end(html);
    html_body_end(html);
    html_end(html);
}
