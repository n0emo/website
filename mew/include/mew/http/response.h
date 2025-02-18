#ifndef MEW_INCLUDE_MEW_HTTP_RESPONSE_H_
#define MEW_INCLUDE_MEW_HTTP_RESPONSE_H_

#include <stddef.h>

#include "mew/alloc.h"
#include "mew/http/common.h"
#include "mew/http/headermap.h"
#include "mew/utils.h"

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
    Allocator alloc;
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

#endif // MEW_INCLUDE_MEW_HTTP_RESPONSE_H_
