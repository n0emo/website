#ifndef HTTP_H_
#define HTTP_H_

#include <stdbool.h>

#include "utils.h"

typedef enum {
    HTTP_OK = 200,
    HTTP_NOT_FOUND = 404,
    HTTP_INTERNAL_SERVER_ERROR = 500,
} HttpStatus;

typedef enum {
    HTTP_GET,
    HTTP_POST,
} HttpMethod;

typedef struct {
    StringView key;
    StringView value;
} Header;

typedef struct {
    Arena *arena;
    Header *items;
    size_t count;
    size_t capacity;
} HeaderMap;

typedef struct {
    Arena arena;
    HttpMethod method;
    StringView resource_path;
    StringView version;
    StringView path;
    StringView query_string;
    HeaderMap headers;
    ArenaStringBuilder body;
    int sd;
} Request;

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
    ResponseBodyKind kind;
    union {
        ArenaStringBuilder bytes;
        ResponseSendFile sendfile;
    } as;
} ResponseBody;

typedef struct {
    HttpStatus status;
    HeaderMap headers;
    ResponseBody body;
    int sd;
} Response;

typedef bool request_handler_t(Request *request, Response *response);

typedef struct {
    int connfd;
    request_handler_t *handler;
} ThreadData;

bool start_server(int port, request_handler_t *handler);
bool accept_connection(int sd, request_handler_t *handler);
bool handle_connection(ThreadData *data);
bool read_request_header_lines(int sd, ArenaStringBuilder *header, ArenaStringBuilder *body);
bool parse_request(int fd, Request *request);
void headers_insert(HeaderMap *map, Header header);
void headers_insert_cstrs(HeaderMap *map, const char *key, const char *value);
const char *status_desc(HttpStatus status);
const char *method_str(HttpMethod method);
bool write_response(int fd, Response response);
bool http_urldecode(StringView sv, ArenaStringBuilder *out);

bool try_serve_dir(Response *response, StringView file, StringView dir);

#endif // HTTP_H_
