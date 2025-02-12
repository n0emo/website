#include "http/router.h"

#include <stdio.h>
#include <strings.h>

#include "str.h"
#include "utils.h"

void http_router_init(HttpRouter *router, Allocator alloc) {
    bzero(router, sizeof(*router));
    router->alloc = alloc;
    // TODO: default fallback
}

bool http_router_handle(HttpRouter *router, HttpRequest *request, HttpResponse *response) {
    for (size_t i = 0; i < router->count; i++) {
        StringView path = request->path;
        HttpRoute route = router->items[i];
        for (size_t j = 0; j < route.pattern_size; j++) {
            StringView segment = sv_chop_by(&path, '/');
            if (segment.count == 0 && path.count == 0) goto next;
            if (!sv_eq_sv(segment, route.pattern[j])) goto next;
        }
        return route.handler(request, response);
    next:;
    }

    printf("Falled back\n");

    return true;
}

void http_route_sv(HttpRouter *router, StringView path, http_request_handler_t *handler, void *user_data) {
    StringView pattern_sv = sv_dup(&router->alloc, path);
    const char *pattern_ptr = pattern_sv.items;

    size_t len = sv_count_char(path, '/') + 1;
    StringView *pattern = mem_alloc(&router->alloc, sizeof(StringView) * len);
    for (size_t i = 0; i < len; i++) {
        StringView segment = sv_chop_by(&pattern_sv, '/');
        pattern[i] = segment;
    }

    HttpRoute route = {
        .pattern_ptr = pattern_ptr,
        .pattern = pattern,
        .pattern_size = len,
        .handler = handler,
        .user_data = user_data,
    };
    ARRAY_APPEND(router, route, &router->alloc);
}

void http_route_cstr(HttpRouter *router, const char *path, http_request_handler_t *handler, void *user_data) {
    http_route_sv(router, cstr_to_sv(path), handler, user_data);
}

void http_route_fallback(HttpRouter *router, http_request_handler_t *handler, void *user_data) {
    router->fallback = handler;
    router->fallback_data = user_data;
}
