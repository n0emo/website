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
    int body_fd;
} Request;

typedef struct {
    HttpStatus status;
    HeaderMap headers;
    ArenaStringBuilder body;
} Response;

typedef bool request_handler_t(Request *request, Response *response);

typedef struct {
    int connfd;
    request_handler_t *handler;
} ThreadData;

bool start_server(int port, request_handler_t *handler);
bool accept_connection(int sd, request_handler_t *handler);
bool handle_connection(ThreadData *data);
bool parse_request(int fd, Request *request);
void headers_insert(HeaderMap *map, Header header);
void headers_insert_cstrs(HeaderMap *map, const char *key, const char *value);
const char *status_desc(HttpStatus status);
bool write_response(int fd, Response response);
bool http_urldecode(StringView sv, ArenaStringBuilder *out);

#endif // HTTP_H_
