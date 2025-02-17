#include "web.h"

#include <dirent.h>
#include <unistd.h>

#include "html.h"
#include "http/http.h"
#include "log.h"
#include "utils.h"
#include "ini.h"

typedef struct {
    StringView dir;
    StringView name;
    StringView desc;
    StringView author;
} BlogDescription;

typedef struct {
    BlogDescription *items;
    size_t count;
    size_t capacity;
} BlogList;

typedef struct Project {
    const char *name;
    const char *desctiption;
    const char *source_code;
    const char *image_src;
    const char *live_demo_src;
} Project;

typedef struct Game {
    const char *name;
    const char *desctiption;
    const char *source_code;
    const char *itch_io;
    const char *live_demo;
} Game;

void response_setup_html(HttpResponse *response);
void render_index(StringBuilder *sb);
void about(Html *html);
void technology(Html *html, const char *name);
void project_list(Html *html);
void project(Html *html, Project project);
void game_list(Html *html);
void game(Html *html, Game game);
void contact_info(Html *html);
void render_blogs(StringBuilder *sb, BlogList list);
bool get_blogs(Allocator alloc, BlogList *list);
void render_blog(StringBuilder *sb, BlogDescription desc, StringView text);
bool get_blog(Allocator alloc, StringView dir, BlogDescription *desc);
bool get_blog_text(Allocator alloc, StringView dir, StringBuilder *blog_text);
void render_music(StringBuilder *sb);
void page_base_begin(Html *html, StringView title);
void page_base_end(Html *html);
void navigation_menu(Html *html);
void footer(Html *html);
void link_item(Html *html, const char *title, const char *destination);

// TODO: parse query
// TODO: handle POST with body
bool handle_root(HttpRequest *request, HttpResponse *response, void *user_data);
bool handle_blogs(HttpRequest *request, HttpResponse *response, void *user_data);
bool handle_blog_by_name(HttpRequest *request, HttpResponse *response, void *user_data);
bool handle_music(HttpRequest *request, HttpResponse *response, void *user_data);
bool handle_assets(HttpRequest *request, HttpResponse *response, void *user_data);

void web_setup_handlers(HttpRouter *router) {
    http_route(router, "/", handle_root);
    http_route(router, "/blogs", handle_blogs);
    http_route(router, "/blogs/:blog", handle_blog_by_name);
    http_route(router, "/music", handle_music);
    http_route_fallback(router, handle_assets, NULL);
}

bool handle_root(HttpRequest *request, HttpResponse *response, void *user_data) {
    response_setup_html(response);
    render_index(&response->body.as.bytes);
    return true;
}

bool handle_blogs(HttpRequest *request, HttpResponse *response, void *user_data) {
    BlogList list = {0};
    if (!get_blogs(request->alloc, &list)) {
        response->status = HTTP_INTERNAL_SERVER_ERROR;
    } else {
        response_setup_html(response);
        render_blogs(&response->body.as.bytes, list);
    }
    return true;
}

bool handle_blog_by_name(HttpRequest *request, HttpResponse *response, void *user_data) {
    StringView *dir = http_path_get(&request->path_params, cstr_to_sv("blog"));
    if (dir == NULL) {
        response->status = HTTP_NOT_FOUND;
        return true;
    }
    BlogDescription blog_desc = {0};
    StringBuilder blog_text = { .alloc = request->alloc, 0 };
    if (
        !get_blog(request->alloc, *dir, &blog_desc) ||
        !get_blog_text(request->alloc, *dir, &blog_text)) {
        response->status = HTTP_INTERNAL_SERVER_ERROR;
    } else {
        response_setup_html(response);
        render_blog(&response->body.as.bytes, blog_desc, sb_to_sv(blog_text));
    }
    return true;
}

bool handle_music(HttpRequest *request, HttpResponse *response, void *user_data) {
    response_setup_html(response);
    render_music(&response->body.as.bytes);
    return true;
}

bool handle_assets(HttpRequest *request, HttpResponse *response, void *user_data) {
    if (try_serve_dir(response, request->path, cstr_to_sv("assets"))) {
        response->status = HTTP_OK;
    } else {
        response->status = HTTP_NOT_FOUND;
    }
    return true;
}

void response_setup_html(HttpResponse *response) {
    response->status = HTTP_OK;
    http_headermap_insert_cstrs(&response->headers, "Content-Type", "text/html; charset=UTF-8");
    response->body.kind = RESPONSE_BODY_BYTES;
    response->body.as.bytes.alloc = response->body.alloc;
}

// TODO: Make webpages design
// TODO: add some CSS
void render_index(StringBuilder *sb) {
    Html html = html_begin();
    page_base_begin(&html, cstr_to_sv("_n0emo website"));

    html_push_class_cstr(&html, "index");
    html_div_begin(&html);

    about(&html);
    project_list(&html);
    game_list(&html);
    contact_info(&html);

    html_div_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

void about(Html *html) {
    html_push_class_cstr(html, "about");
    html_push_attribute_cstrs(html, "id", "about");
    html_section_begin(html);

    html_h1_begin(html);
    html_text_cstr(html, "About");
    html_h1_end(html);

    html_push_class_cstr(html, "content");
    html_div_begin(html);

    html_push_class_cstr(html, "profile");
    html_div_begin(html);

    html_push_attribute_cstrs(html, "src", "/logos/Person.svg");
    html_img_short(html);

    html_p_begin(html);
    html_text_cstr(
        html,
        "I'm Albert Shefner. I'm a "
        "software engineer interested in many areas, such as: low-level, "
        "system and audio programming; web and game development. Can work "
        "with any enough documented piece of technology."
    );
    html_p_end(html);

    html_div_end(html);

    static const char *technologies[] = { "WebGPU", "C", "Tokio", "Git", "Python", "C#", "Rust", "C++", "JavaScript", "Clojure", "Go" };
    html_push_class_cstr(html, "technology-grid");
    html_div_begin(html);
    for (size_t i = 0; i < sizeof(technologies) / sizeof(technologies[0]); i++) {
        technology(html, technologies[i]);
    }
    html_div_end(html);

    html_div_end(html);

    html_section_end(html);
}

void technology(Html *html, const char *name) {
    html_push_class_cstr(html, "technology-wrap");
    html_div_begin(html);
    html_push_class_cstr(html, "technology");
    html_div_begin(html);

    StringBuilder src = { .alloc = html->alloc, 0 };
    sb_append_cstr(&src, "/logos/");
    http_urlencode(cstr_to_sv(name), &src);
    sb_append_cstr(&src, ".svg\0");
    html_push_attribute_cstrs(html, "src", src.items);
    html_tag_short(html, "img");

    html_p_begin(html);
    html_text_cstr(html, name);
    html_p_end(html);

    html_div_end(html);
    html_div_end(html);
}

void project_list(Html *html) {
    html_push_attribute_cstrs(html, "id", "projects");
    html_section_begin(html);

    html_h1_begin(html);
    html_text_cstr(html, "Projects");
    html_h1_end(html);

    const static Project projects[] = {
        (Project) {
            .name = "Compressor",
            .desctiption = "Dynamic compressor CLAP plugin in C",
            .source_code = "https://github.com/n0emo/compressor",
            .image_src = NULL,
            .live_demo_src = NULL,
        },
        (Project) {
            .name = "Hyperclip",
            .desctiption = "Distortion CLAP/VST plugin",
            .source_code = "https://github.com/n0emo/hyperclip",
            .image_src = NULL,
            .live_demo_src = NULL,
        },
        (Project) {
            .name = "NBS",
            .desctiption = "Build system to build C++ using C++",
            .source_code = "https://github.com/n0emo/compressor",
            .image_src = NULL,
            .live_demo_src = NULL,
        },
    };

    html_push_class_cstr(html, "project-list");
    html_div_begin(html);
    for (size_t i = 0; i < sizeof(projects) / sizeof(*projects); i++) {
        project(html, projects[i]);
    }
    html_div_end(html);

    html_section_end(html);
}

void project(Html *html, Project project) {
    html_push_class_cstr(html, "project");
    html_div_begin(html);

    html_hyperlink_cstr(html, project.name, project.source_code);
    html_p_begin(html);
    html_text_cstr(html, project.desctiption);
    html_p_end(html);

    html_div_end(html);
}

void game_list(Html *html) {
    html_push_attribute_cstrs(html, "id", "games");
    html_section_begin(html);

    html_h1_begin(html);
    html_text_cstr(html, "Games");
    html_h1_end(html);

    const static Game games[] = {
        (Game) {
            .name = "Excuse me, what?",
            .desctiption = "Chaotic top-down shooter game",
            .source_code = "https://github.com/n0emo/chaos",
            .itch_io = "https://n0emo.itch.io/excuse-me-what",
            .live_demo = NULL,
        }
    };

    html_push_class_cstr(html, "game-list");
    html_div_begin(html);
    for (size_t i = 0; i < sizeof(games) / sizeof(*games); i++) {
        game(html, games[i]);
    }
    html_div_end(html);

    html_section_end(html);
}

void game(Html *html, Game game) {
    html_push_class_cstr(html, "game");
    html_div_begin(html);

    html_push_class_cstr(html, "title");
    html_hyperlink_cstr(html, game.name, game.source_code);
    html_p_begin(html);
    html_text_cstr(html, game.desctiption);
    html_p_end(html);

    if (game.itch_io) {
        html_hyperlink_cstr(html, "itch.io", game.itch_io);
    }

    html_div_end(html);
}

void contact_info(Html *html) {
    html_push_attribute_cstrs(html, "id", "contact");
    html_section_begin(html);

    html_h1_begin(html);
    html_text_cstr(html, "Contact");
    html_h1_end(html);

    html_p_begin(html);
    html_text_cstr(html, "Email: solaris945[at]gmail.com");
    html_p_end(html);

    html_p_begin(html);
    html_text_cstr(html, "Telegram: @omega_lolxd");
    html_p_end(html);

    html_section_end(html);
}

// TODO: render markdown to HTML
void render_blogs(StringBuilder *sb, BlogList list) {
    Html html = html_begin();
    page_base_begin(&html, cstr_to_sv("Blogs"));

    html_h1_begin(&html);
    html_text_cstr(&html, "Recent blog posts:");
    html_h1_end(&html);

    for (size_t i = 0; i < list.count; i++) {
        BlogDescription d = list.items[i];
        html_div_begin(&html);

        html_h2_begin(&html);
        const char *blog_destination = mem_sprintf(
            html.alloc,
            "/blogs/" SV_FMT,
            SV_ARG(d.dir)
        );
        html_hyperlink(&html, d.name, cstr_to_sv(blog_destination));
        html_h2_end(&html);

        const char *blog_desc = mem_sprintf(
            html.alloc,
            SV_FMT " - by " SV_FMT,
            SV_ARG(d.desc), SV_ARG(d.author)
        );

        html_p_begin(&html);
        html_text_cstr(&html, blog_desc);
        html_p_end(&html);

        html_div_end(&html);
    }

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

bool get_blogs(Allocator alloc, BlogList *list) {
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
            IniSection *metadata = ini_get_section(ini, cstr_to_sv("Metadata"));
            if (metadata == NULL) continue;

            IniItem *name = ini_get_item(*metadata, cstr_to_sv("name"));
            if (name == NULL) continue;

            IniItem *desc = ini_get_item(*metadata, cstr_to_sv("desc"));
            if (desc == NULL) continue;

            IniItem *author = ini_get_item(*metadata, cstr_to_sv("author"));
            if (author == NULL) continue;

            BlogDescription blog = {
                .dir = cstr_to_sv(mem_strdup(alloc, d)),
                .name = sv_dup(alloc, name->value),
                .desc = sv_dup(alloc, desc->value),
                .author = sv_dup(alloc, author->value),
            };

            ARRAY_APPEND(list, blog, alloc);
        } else {
            log_error("Error parsing %s", path);
        }

        sb.count = 0;
    }

defer:
    closedir(dir);
    return result;
}

void render_blog(StringBuilder *sb, BlogDescription desc, StringView text) {
    Html html = html_begin();

    page_base_begin(&html, desc.name);

    html_hyperlink_cstr(&html, "Back", "/blogs");

    html_h1_begin(&html);
    html_text(&html, desc.name);
    html_h1_end(&html);

    html_p_begin(&html);
    html_text(&html, text);
    html_p_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}

bool get_blog(Allocator alloc, StringView dir, BlogDescription *blog_desc) {
    StringBuilder sb = { .alloc = alloc, 0 };
    const char *path = mem_sprintf(alloc, "blogs/" SV_FMT "/metadata.ini", SV_ARG(dir));

    if (!read_file_to_sb(path, &sb)) return false;
    Ini ini = { .alloc = alloc, {0} };
    if (!parse_ini(sb_to_sv(sb), &ini)) return false;

    // TODO: move to separate function
    IniSection *metadata = ini_get_section(ini, cstr_to_sv("Metadata"));
    if (metadata == NULL) return false;

    IniItem *name = ini_get_item(*metadata, cstr_to_sv("name"));
    if (name == NULL) return false;

    IniItem *desc = ini_get_item(*metadata, cstr_to_sv("desc"));
    if (desc == NULL) return false;

    IniItem *author = ini_get_item(*metadata, cstr_to_sv("author"));
    if (author == NULL) return false;

    *blog_desc = (BlogDescription ) {
        .dir = dir,
        .name = sv_dup(alloc, name->value),
        .desc = sv_dup(alloc, desc->value),
        .author = sv_dup(alloc, author->value),
    };

    return true;
}

bool get_blog_text(Allocator alloc, StringView dir, StringBuilder *blog_text) {
    const char *path = mem_sprintf(alloc, "blogs/" SV_FMT "/en.md", SV_ARG(dir));
    return read_file_to_sb(path, blog_text);
}

void render_music(StringBuilder *sb) {
    Html html = html_begin();
    page_base_begin(&html, cstr_to_sv("Music"));

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

void page_base_begin(Html *html, StringView title) {
    html_head_begin(html);
    html_title(html, title);
    html_push_attribute_cstrs(html, "name", "viewport");
    html_push_attribute_cstrs(html, "content", "width=device-width");
    html_meta_short(html);
    html_push_attribute_cstrs(html, "rel", "stylesheet");
    html_push_attribute_cstrs(html, "href", "/styles/base.css");
    html_link_short(html);
    html_head_end(html);

    html_body_begin(html);
    navigation_menu(html);
    html_main_begin(html);
}

void page_base_end(Html *html) {
    html_main_end(html);
    footer(html);
    html_body_end(html);
    html_end(html);
}

void navigation_menu(Html *html) {
    html_nav_begin(html);
    html_ul_begin(html);
    link_item(html, "About", "#about");
    link_item(html, "Projects", "#projects");
    link_item(html, "Games", "#games");
    link_item(html, "Contact", "#contact");

    html_ul_end(html);
    html_nav_end(html);
}

void footer(Html *html) {
    html_footer_begin(html);
    html_push_class_cstr(html, "footer-text");
    html_div_begin(html);
    html_p_begin(html);
    html_text_cstr(html, "Ancient technologies are being used to show this page for you.");
    html_p_end(html);
    html_hyperlink_cstr(html, "Source code", "https://github.com/n0emo/website");
    html_div_end(html);
    html_footer_end(html);
}

void link_item(Html *html, const char *title, const char *destination) {
    html_li_begin(html);
    html_hyperlink_cstr(html, title, destination);
    html_li_end(html);
}

