// TODO: radix tree
// TODO: middleware
// TODO: nest routers
// TODO: nest handlers
#ifndef MEW_INCLUDE_MEW_HTTP_ROUTER_H_
#define MEW_INCLUDE_MEW_HTTP_ROUTER_H_

#include "mew/alloc.h"
#include "mew/http/handler.h"
#include "mew/str.h"

#include <stddef.h>

typedef struct HttpMatcherState {
    struct HttpMatcherState *states; // should be exactle 256 states or NULL
    const void *data;
    bool has_data;
} HttpMatcherState;

typedef struct HttpMatcher {
    HttpMatcherState *states;
} HttpMatcher;

typedef struct HttpMatch {
    const void *data;
    StringView path_params[4];
    StringView *additional_path_params;
    StringView remaining_path;
} HttpMatch;

void http_matcher_init(HttpMatcher *matcher);
void http_matcher_register(HttpMatcher *matcher, StringView path, const void *data);
bool http_matcher_find(HttpMatcher *matcher, StringView path, HttpMatch *match_result);

typedef struct HttpRouter {
    HttpMatcher matcher;
} HttpRouter;

/*
typedef struct HttpRoute {
    const char *pattern_ptr;
    StringView *pattern;
    size_t pattern_size;
    HttpRequestHandler handler;
} HttpRoute;

typedef struct HttpRouter {
    Allocator alloc;

    HttpRoute *items;
    size_t count;
    size_t capacity;
    void *user_data;

    http_request_handle_func_t *fallback;
    void *fallback_data;
} HttpRouter;
*/

void http_router_init(HttpRouter *router, void *user_data, Allocator alloc);
bool http_router_handle(HttpRouter *router, HttpRequest *request, HttpResponse *response);
void http_route_sv(HttpRouter *router, StringView path, http_request_handle_func_t *handler);
void http_route_cstr(HttpRouter *router, const char *path, http_request_handle_func_t *handler);
void http_route_handler_sv(HttpRouter *router, StringView path, HttpRequestHandler handler);
void http_route_handler_cstr(HttpRouter *router, const char *path, HttpRequestHandler handler);
void http_route_fallback(HttpRouter *router, http_request_handle_func_t *handler, void *user_data);

#define http_route(router, path, handler) _Generic((path), \
    StringView: http_route_sv, \
    char *: http_route_cstr \
)(router, path, handler)

#define http_route_handler(router, path, handler) _Generic((path), \
    StringView: http_route_handler_sv, \
    char *: http_route_handler_cstr \
)(router, path, handler)

#endif // MEW_INCLUDE_MEW_HTTP_ROUTER_H_
