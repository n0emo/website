#ifndef HTTP_ROUTER_H_
#define HTTP_ROUTER_H_

#include "alloc.h"
#include "http/request.h"
#include "http/response.h"
#include "str.h"

#include <stddef.h>

typedef bool http_request_handler_t(HttpRequest *request, HttpResponse *response);

typedef struct HttpRoute {
    const char *pattern_ptr;
    StringView *pattern;
    size_t pattern_size;
    http_request_handler_t *handler;
    void *user_data;
} HttpRoute;

typedef struct HttpRouter {
    Allocator alloc;

    HttpRoute *items;
    size_t count;
    size_t capacity;

    http_request_handler_t *fallback;
    void *fallback_data;
} HttpRouter;

void http_router_init(HttpRouter *router, Allocator alloc);
bool http_router_handle(HttpRouter *router, HttpRequest *request, HttpResponse *response);
void http_route_sv(HttpRouter *router, StringView path, http_request_handler_t *handler, void *user_data);
void http_route_cstr(HttpRouter *router, const char *path, http_request_handler_t *handler, void *user_data);
void http_route_fallback(HttpRouter *router, http_request_handler_t *handler, void *user_data);

#define http_route(router, path, handler, user_data) _Generic((path), \
                                                              StringView: http_route_sv, \
                                                              char *: http_route_cstr \
                                                    )(router, path, handler, user_data)

#endif // HTTP_ROUTER_H_
