#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include <stddef.h>

#include "alloc.h"
#include "http/common.h"
#include "http/headermap.h"
#include "utils.h"

typedef enum {
    RESPONSE_BODY_NONE = 0,
    RESPONSE_BODY_BYTES,
    RESPONSE_BODY_SENDFILE,
} ResponseBodyKind;

typedef struct {
    const char *path;
    size_t size;
} ResponseSendFile;

typedef struct {
    Arena *arena;
    Allocator *alloc;
    ResponseBodyKind kind;
    union {
        StringBuilder bytes;
        ResponseSendFile sendfile;
    } as;
} ResponseBody;

typedef struct {
    HttpStatus status;
    HttpHeaderMap headers;
    ResponseBody body;
    int sd;
} HttpResponse;

bool http_response_write(HttpResponse *response, int fd);

#endif //  HTTP_RESPONSE_H_
