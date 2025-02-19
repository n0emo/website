#include "web.h"

#include <dirent.h>
#include <unistd.h>

#include "mew/html.h"
#include "mew/http/http.h"

#include "pages/index.h"
#include "pages/blogs.h"

void response_setup_html(HttpResponse *response);

void render_music(StringBuilder *sb);
void page_base_begin(Html *html, StringView title);
void page_base_end(Html *html);

// TODO: parse query
// TODO: handle POST with body
bool handle_root(HttpRequest *request, HttpResponse *response);
bool handle_blogs(HttpRequest *request, HttpResponse *response);
bool handle_blog_by_name(HttpRequest *request, HttpResponse *response);
bool handle_music(HttpRequest *request, HttpResponse *response);
bool handle_assets(HttpRequest *request, HttpResponse *response);

void web_setup_handlers(HttpRouter *router) {
    http_route(router, "/", handle_root);
    http_route(router, "/blogs", handle_blogs);
    http_route(router, "/blogs/:blog", handle_blog_by_name);
    http_route(router, "/music", handle_music);
    http_route_fallback(router, handle_assets, NULL);
}

bool handle_root(HttpRequest *request, HttpResponse *response) {
    response_setup_html(response);
    page_index(&response->body.as.bytes);
    return true;
}

bool handle_blogs(HttpRequest *request, HttpResponse *response) {
    BlogList list = {0};
    if (!get_blogs(request->ctx.alloc, &list)) {
        response->status = HTTP_INTERNAL_SERVER_ERROR;
    } else {
        response_setup_html(response);
        blogs(&response->body.as.bytes, list);
    }
    return true;
}

bool handle_blog_by_name(HttpRequest *request, HttpResponse *response) {
    StringView *dir = http_path_get(&request->ctx.path_params, cstr_to_sv("blog"));
    if (dir == NULL) {
        response->status = HTTP_NOT_FOUND;
        return true;
    }
    Blog blog_desc = {0};
    StringBuilder blog_text = { .alloc = request->ctx.alloc, 0 };
    if (
        !get_blog(request->ctx.alloc, *dir, &blog_desc) ||
        !get_blog_text(request->ctx.alloc, *dir, &blog_text)) {
        response->status = HTTP_INTERNAL_SERVER_ERROR;
    } else {
        response_setup_html(response);
        blog(&response->body.as.bytes, blog_desc, sb_to_sv(blog_text));
    }
    return true;
}

bool handle_music(HttpRequest *request, HttpResponse *response) {
    response_setup_html(response);
    render_music(&response->body.as.bytes);
    return true;
}

bool handle_assets(HttpRequest *request, HttpResponse *response) {
    if (try_serve_dir(response, request->ctx.path, cstr_to_sv("assets"))) {
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

