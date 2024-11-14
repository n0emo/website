#include "web.h"

#include "html.h"
#include "log.h"
#include "utils.h"

void response_setup_html(Response *response) {
    response->status = HTTP_OK;
    headers_insert_cstrs(&response->headers, "Content-Type", "text/html; charset=UTF-8");
    response->body.kind = RESPONSE_BODY_BYTES;
    response->body.as.bytes.arena = response->body.arena;
}

// TODO: parse query
// TODO: handle POST with body
bool handle_request(Request *request, Response *response) {
    if (sv_eq_cstr(request->path, "/")) {
        response_setup_html(response);
        render_index(&response->body.as.bytes);
    } else if (sv_eq_cstr(request->path, "/blogs")) {
        response_setup_html(response);
        render_blogs(&response->body.as.bytes);
    } else if (sv_eq_cstr(request->path, "/music")) {
        response_setup_html(response);
        render_music(&response->body.as.bytes);
    } else if (try_serve_dir(response, request->path, cstr_to_sv("assets"))) {
        response->status = HTTP_OK;
    } else {
        response->status = HTTP_NOT_FOUND;
    }

    log_debug(
        "%s " SV_FMT ": %d %s",
        method_str(request->method),
        SV_ARG(request->path),
        // TODO
        response->status,
        status_desc(response->status)
    );

    return true;
}

// TODO: Make webpages design
// TODO: add some CSS
void render_index(ArenaStringBuilder *sb) {
    Html html = {0};
    page_base_begin(&html, "_n0emo website");

    html_h1_begin(&html);
    html_text(&html, "Welcome to the _n0emo's personal website!");
    html_h1_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

// TODO: render markdown to HTML
void render_blogs(ArenaStringBuilder *sb) {
    Html html = {0};
    page_base_begin(&html, "Blogs");

    html_h1_begin(&html);
    html_text(&html, "Recent blog posts:");
    html_h1_end(&html);

    html_ul_begin(&html);
    for (size_t i = 10; i < 200; i++) {
        html_li_begin(&html);
        html_text(&html, arena_sprintf(&html.arena, "Blog %zu", i));
        html_li_end(&html);
    }
    html_ul_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

void link(Html *html, const char *title, const char *destination) {
    html_push_attribute_cstrs(html, "href", destination);
    html_a_begin(html);
    html_pop_attributes(html, 1);
    html_text(html, title);
    html_a_end(html);
}

void link_item(Html *html, const char *title, const char *destination) {
    html_li_begin(html);
    link(html, title, destination);
    html_li_end(html);
}

void render_music(ArenaStringBuilder *sb) {
    Html html = {0};
    page_base_begin(&html, "Music");

    html_h1_begin(&html);
    html_text(&html, "There will be some stuff about my music");
    html_h1_end(&html);

    html_h2_begin(&html);
    html_text(&html, "Listen:");
    html_h2_end(&html);

    html_ul_begin(&html);
    link_item(&html, "YouTube", "https://youtube.com/channel/UCuYpUcLPU_0xn1jZJrRiyEQ?si=8SOVgU9d5C0KHAzD");
    link_item(&html, "Yandex Music", "https://music.yandex.ru/artist/17650218");
    link_item(&html, "VK", "https://vk.com/artist/_n0emo");
    html_ul_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

void navigation_menu(Html *html) {
    html_nav_begin(html);
    html_ul_begin(html);
    link_item(html, "Home", "/");
    link_item(html, "Blogs", "/blogs");
    link_item(html, "Music", "/music");

    html_push_class_cstr(html, "nav-right");
    link_item(html, "GitHub", "https://github.com/n0emo");
    html_pop_classes(html, 1);

    html_ul_end(html);
    html_nav_end(html);
}

void footer(Html *html) {
    html_footer_begin(html);
    html_push_class_cstr(html, "footer-text");
    html_div_begin(html);
    html_pop_classes(html, 1);
    html_p_begin(html);
    html_text(html, "Ancient technologies are being used to show this page for you.");
    html_p_end(html);
    html_push_attribute_cstrs(html, "href", "https://github.com/n0emo/website");
    html_a_begin(html);
    html_text(html, "Source code");
    html_a_end(html);
    html_pop_attributes(html, 1);
    html_div_end(html);
    html_footer_end(html);
}

void page_base_begin(Html *html, char *title) {
    html_begin(html);

    html_head_begin(html);
    html_title(html, title);
    html_push_attribute_cstrs(html, "rel", "stylesheet");
    html_push_attribute_cstrs(html, "href", "/styles/base.css");
    html_link(html);
    html_pop_attributes(html, 2);
    html_head_end(html);

    html_body_begin(html);
    navigation_menu(html);
    html_push_class_cstr(html, "main-content");
    html_div_begin(html);
    html_pop_classes(html, 1);
}

void page_base_end(Html *html) {
    html_div_end(html);
    footer(html);
    html_body_end(html);
    html_end(html);
}
