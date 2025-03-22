#ifndef MEW_INCLUDE_MEW_HTTP_RESPONSE_H_
#define MEW_INCLUDE_MEW_HTTP_RESPONSE_H_

#include <stddef.h>

#include "mew/alloc.h"
#include "mew/http/common.h"
#include "mew/http/headermap.h"
#include "mew/os/socket.h"
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
} HttpResponse;

/**
 * Writes entire `response` to `stream`.
 */
bool http_response_write(HttpResponse *response, MewTcpStream stream);

/**
 * Sets response `kind` to `RESPONSE_BODY_BYTES` and initializes `as.bytes`.
 * This function expects response body `kind` to be `RESPONSE_BODY_NONE`.
 */
void http_response_body_set_bytes(HttpResponse *response);

/**
 * Sets response `kind` to `RESPONSE_BODY_SENDFILE` and initializes `as.sendfile`.
 * This function expects response body `kind` to be `RESPONSE_BODY_NONE`.
 */
void http_response_body_set_sendfile(HttpResponse *response, ResponseSendFile sendfile);

/**
 * Sets response `status` to `HTTP_OK` and `Content-Type` to `text/html`.
 * This function expects response body `kind` to be `RESPONSE_BODY_BYTES` and
 * `as.bytes` to be valid non-empty HTML.
 */
void http_response_set_html(HttpResponse *response);

/**
 * Sets response `status` to `HTTP_NOT_FOUND`.
 */
void http_response_not_found(HttpResponse *response);

/**
 * Sets response `status` to `HTTP_INTERNAL_SERVER_ERROR`.
 */
void http_response_set_internal_server_error(HttpResponse *response);

#endif // MEW_INCLUDE_MEW_HTTP_RESPONSE_H_
