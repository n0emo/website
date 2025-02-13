#include "http/router.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "str.h"
#include "utils.h"

int compare_routes(const void * a, const void * b);

void http_router_init(HttpRouter *router, void *user_data, Allocator alloc) {
    bzero(router, sizeof(*router));
    router->alloc = alloc;
    router->user_data = user_data;
    // TODO: default fallback
}

bool http_router_handle(HttpRouter *router, HttpRequest *request, HttpResponse *response) {
    request->path_params.is_set = true;

    for (size_t i = 0; i < router->count; i++) {
        StringView path = request->path;
        if (path.items[0] == '/') path = sv_slice_from(path, 1);
        HttpRoute route = router->items[i];
        for (size_t j = 0; j < route.pattern_size; j++) {
            StringView segment = sv_chop_by(&path, '/');
            StringView pattern = route.pattern[j];

            if (segment.count == 0 && path.count == 0) goto next;

            if (pattern.items[0] == ':') {
                StringView key = sv_slice_from(pattern, 1);
                http_path_set(&request->path_params, key, segment);
                continue;
            }

            if (!sv_eq_sv(segment, pattern)) goto next;
        }
        if (path.count > 0) goto next;
        return route.handler.handler(request, response, route.handler.user_data);
    next:;
    }

    return router->fallback(request, response, router->fallback_data);
}

void http_route_sv(HttpRouter *router, StringView path, http_request_handle_func_t *handler) {
    http_route_handler_sv(router, path, (HttpRequestHandler) { handler, router->user_data });
}

void http_route_cstr(HttpRouter *router, const char *path, http_request_handle_func_t *handler) {
    http_route_sv(router, cstr_to_sv(path), handler);
}

void http_route_handler_cstr(HttpRouter *router, const char *path, HttpRequestHandler handler) {
    http_route_handler_sv(router, cstr_to_sv(path), handler);
}

void http_route_handler_sv(HttpRouter *router, StringView path, HttpRequestHandler handler) {
    assert(path.items[0] == '/' && "Paths must start with '/'");
    StringView pattern_sv = sv_dup(router->alloc, sv_slice_from(path, 1));
    const char *pattern_ptr = pattern_sv.items;

    size_t len = sv_count_char(pattern_sv, '/') + 1;
    if (pattern_sv.count == 0) len = 0;
    StringView *pattern = mem_alloc(router->alloc, sizeof(StringView) * len);
    for (size_t i = 0; i < len; i++) {
        StringView segment = sv_chop_by(&pattern_sv, '/');
        pattern[i] = segment;
    }

    HttpRoute route = {
        .pattern_ptr = pattern_ptr,
        .pattern = pattern,
        .pattern_size = len,
        .handler = handler,
    };
    ARRAY_APPEND(router, route, router->alloc);

    qsort(router->items, router->count, sizeof(*router->items), compare_routes);
}

void http_route_fallback(HttpRouter *router, http_request_handle_func_t *handler, void *user_data) {
    router->fallback = handler;
    router->fallback_data = user_data;
}

int compare_routes(const void * a, const void * b) {
    HttpRoute *ra = (HttpRoute *) a;
    HttpRoute *rb = (HttpRoute *) b;
    return rb->pattern_size - ra->pattern_size;
}
