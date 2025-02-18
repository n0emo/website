#ifndef MEW_INCLUDE_MEW_HTTP_FS_H_
#define MEW_INCLUDE_MEW_HTTP_FS_H_

#include <stdbool.h>

#include "mew/http/response.h"

// TODO: cached serve dir
// TODO: as handler interface
bool try_serve_dir(HttpResponse *response, StringView file, StringView dir);

#endif // MEW_INCLUDE_MEW_HTTP_FS_H_
