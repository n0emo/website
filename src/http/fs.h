#ifndef HTTP_FS_H_
#define HTTP_FS_H_

#include "utils.h"
#include "http/response.h"

#include <stdbool.h>

bool try_serve_dir(HttpResponse *response, StringView file, StringView dir);

#endif // HTTP_FS_H_

