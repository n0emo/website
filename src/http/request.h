#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "alloc.h"
#include "http/common.h"
#include "http/headermap.h"
#include "utils.h"

typedef struct {
    Arena arena;
    Allocator alloc;
    HttpMethod method;
    StringView resource_path;
    StringView version;
    StringView path;
    StringView query_string;
    HttpHeaderMap headers;
    StringBuilder body;
    int sd;
} HttpRequest;

bool http_request_parse(HttpRequest *request, int fd);

#endif // HTTP_REQUEST_H_
