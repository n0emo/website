#ifndef MEW_INCLUDE_MEW_HTTP_HANDLER_H_
#define MEW_INCLUDE_MEW_HTTP_HANDLER_H_

#include "mew/http/request.h"
#include "mew/http/response.h"

/**
 * Returns false in case of the error
 */
typedef bool http_request_handle_func_t(HttpRequest *request, HttpResponse *response, void *user_data);

typedef struct HttpRequestHandler {
    http_request_handle_func_t *handler;
    void *user_data;
} HttpRequestHandler;

#endif // MEW_INCLUDE_MEW_HTTP_HANDLER_H_
