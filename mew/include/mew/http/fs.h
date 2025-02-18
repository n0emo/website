#ifndef MEW_INCLUDE_MEW_HTTP_FS_H_
#define MEW_INCLUDE_MEW_HTTP_FS_H_

#include "mew/utils.h"
#include "mew/http/response.h"

#include <stdbool.h>

bool try_serve_dir(HttpResponse *response, StringView file, StringView dir);

#endif // MEW_INCLUDE_MEW_HTTP_FS_H_
