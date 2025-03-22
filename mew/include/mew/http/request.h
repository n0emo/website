#ifndef MEW_INCLUDE_MEW_HTTP_REQUEST_H_
#define MEW_INCLUDE_MEW_HTTP_REQUEST_H_

#include "mew/alloc.h"
#include "mew/containers/hashmap.h"
#include "mew/http/common.h"
#include "mew/http/headermap.h"
#include "mew/os/socket.h"
#include "mew/utils.h"

typedef struct HttpPathParams {
    HashMap map;
    bool is_set;
} HttpPathParams;

void http_path_init(HttpPathParams *params, Allocator alloc);
void http_path_set(HttpPathParams *params, StringView key, StringView value);
StringView *http_path_get(HttpPathParams *params, StringView key);
void http_path_destroy(HttpPathParams *params);

typedef struct HttpRequestContext {
    Allocator alloc;
    StringView path;
    StringView query_string;
    HttpPathParams path_params;
    void *user_data;
} HttpRequestContext;

typedef struct {
    HttpMethod method;
    StringView resource_path;
    StringView version;
    HttpHeaderMap headers;
    StringBuilder body;

    HttpRequestContext ctx;
} HttpRequest;

bool http_request_init(HttpRequest *request, Allocator alloc);
bool http_request_parse(HttpRequest *request, MewTcpStream stream);

#endif // MEW_INCLUDE_MEW_HTTP_REQUEST_H_
