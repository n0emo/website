#include "web.h"

#include <dirent.h>

#include "html.h"
#include "log.h"
#include "utils.h"
#include "ini.h"

typedef struct {
    StringView name;
    Ini ini;
} BlogDescription;

typedef struct {
    BlogDescription *items;
    size_t count;
    size_t capacity;
} BlogList;

void response_setup_html(Response *response);
void render_index(StringBuilder *sb);
void render_blogs(StringBuilder *sb, BlogList list);
bool get_blogs(Allocator *alloc, BlogList *list);
void render_music(StringBuilder *sb);
void page_base_begin(Html *html, char *title);
void page_base_end(Html *html);
void navigation_menu(Html *html);
void footer(Html *html);
void link_item(Html *html, const char *title, const char *destination);
void link(Html *html, const char *title, const char *destination);

// TODO: parse query
// TODO: handle POST with body
bool handle_request(Request *request, Response *response) {
    if (sv_eq_cstr(request->path, "/")) {
        response_setup_html(response);
        render_index(&response->body.as.bytes);
    } else if (sv_eq_cstr(request->path, "/blogs")) {
        BlogList list = {0};
        if (!get_blogs(&request->alloc, &list)) {
            response->status = HTTP_INTERNAL_SERVER_ERROR;
        } else {
            response_setup_html(response);
            render_blogs(&response->body.as.bytes, list);
        }
    } else if (sv_starts_with(request->path, cstr_to_sv("/blogs/"))) {
        response->status = HTTP_INTERNAL_SERVER_ERROR;
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

void response_setup_html(Response *response) {
    response->status = HTTP_OK;
    headers_insert_cstrs(&response->headers, "Content-Type", "text/html; charset=UTF-8");
    response->body.kind = RESPONSE_BODY_BYTES;
    response->body.as.bytes.alloc = response->body.alloc;
}

// TODO: Make webpages design
// TODO: add some CSS
void render_index(StringBuilder *sb) {
    Html html = html_begin();
    page_base_begin(&html, "_n0emo website");

    html_h1_begin(&html);
    html_text_cstr(&html, "Welcome to the _n0emo's personal website!");
    html_h1_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

// TODO: render markdown to HTML
void render_blogs(StringBuilder *sb, BlogList list) {
    Html html = html_begin();
    page_base_begin(&html, "Blogs");

    html_h1_begin(&html);
    html_text_cstr(&html, "Recent blog posts:");
    html_h1_end(&html);

    for (size_t i = 0; i < list.count; i++) {
        BlogDescription d = list.items[i];
        html_div_begin(&html);

        html_h2_begin(&html);
        html_text(&html, d.name);
        html_h2_end(&html);

        for (size_t j = 0; j < d.ini.sections.count; j++) {
            IniSection s = d.ini.sections.items[j];
            html_h3_begin(&html);
            html_text(&html, s.name);
            html_h3_end(&html);

            html_ul_begin(&html);
            for (size_t k = 0; k < s.items.count; k++) {
                IniItem item = s.items.items[k];
                html_li_begin(&html);
                html_text_cstr(
                    &html, mem_sprintf(
                        &html.alloc,
                        SV_FMT ": " SV_FMT,
                        SV_ARG(item.key), SV_ARG(item.value)
                    )
                );
                html_li_end(&html);
            }

            html_ul_end(&html);
        }

        html_div_end(&html);
    }

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

bool get_blogs(Allocator *alloc, BlogList *list) {
    bool result = true;
    DIR *dir;
    struct dirent *dirent;
    StringBuilder sb = { .alloc = alloc, 0 };

    dir = opendir("blogs");
    if (dir == NULL) return_defer(false);

    while ((dirent = readdir(dir))) {
        const char *d = dirent->d_name;
        if (strncmp(d, ".", 1) == 0 || strncmp(d, "..", 2) == 0) continue;
        const char *path = mem_sprintf(alloc, "blogs/%s/metadata.ini", d);

        if (!read_file_to_sb(path, &sb)) {
            sb.count = 0;
            continue;
        }

        Ini ini = { .alloc = alloc, .sections = {0} };
        StringView sv = {
            .items = mem_memdup(alloc, sb.items, sb.count),
            .count = sb.count,
        };
        if (parse_ini(sv, &ini)) {
            BlogDescription desc = {
                .name = cstr_to_sv(mem_strdup(alloc, d)),
                .ini = ini,
            };

            ARRAY_APPEND(list, desc, alloc);
        } else {
            log_error("Error parsing %s", path);
        }

        sb.count = 0;
    }

    for (size_t k = 0; k < list->count; k++) {
        Ini ini = list->items[k].ini;

        for (size_t i = 0; i < ini.sections.count; i++) {
            for (size_t j = 0; j < ini.sections.items[i].items.count; j++) {
                IniItem item = ini.sections.items[i].items.items[j];
                log_info(SV_FMT ": " SV_FMT, SV_ARG(item.key), SV_ARG(item.value));
            }
        }
    }

defer:
    closedir(dir);
    return result;
}

void render_music(StringBuilder *sb) {
    Html html = html_begin();
    page_base_begin(&html, "Music");

    html_h1_begin(&html);
    html_text_cstr(&html, "There will be some stuff about my music");
    html_h1_end(&html);

    html_h2_begin(&html);
    html_text_cstr(&html, "Listen:");
    html_h2_end(&html);

    html_ul_begin(&html);
    link_item(&html, "YouTube", "https://youtube.com/channel/UCuYpUcLPU_0xn1jZJrRiyEQ?si=8SOVgU9d5C0KHAzD");
    link_item(&html, "Yandex Music", "https://music.yandex.ru/artist/17650218");
    link_item(&html, "VK", "https://vk.com/artist/_n0emo");
    html_ul_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

void page_base_begin(Html *html, char *title) {
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
    html_text_cstr(html, "Ancient technologies are being used to show this page for you.");
    html_p_end(html);
    html_push_attribute_cstrs(html, "href", "https://github.com/n0emo/website");
    html_a_begin(html);
    html_text_cstr(html, "Source code");
    html_a_end(html);
    html_pop_attributes(html, 1);
    html_div_end(html);
    html_footer_end(html);
}

void link_item(Html *html, const char *title, const char *destination) {
    html_li_begin(html);
    link(html, title, destination);
    html_li_end(html);
}

void link(Html *html, const char *title, const char *destination) {
    html_push_attribute_cstrs(html, "href", destination);
    html_a_begin(html);
    html_pop_attributes(html, 1);
    html_text_cstr(html, title);
    html_a_end(html);
}
