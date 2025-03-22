#include "mew/http/fs.h"

#include "mew/os/fs.h"

#include <string.h>

bool try_serve_dir(HttpResponse *response, StringView file, StringView dir) {
    if (file.items[0] == '/') file = sv_slice_from(file, 1);

    char *path = mem_sprintf(
        response->body.alloc,
        SV_FMT "/" SV_FMT,
        SV_ARG(dir), SV_ARG(file)
    );

    if (strncmp(path, "../", 3) == 0 ||
        strncmp(path + strlen(path) - 3, "/..", 3) == 0 ||
        strstr(path, "/../") != NULL) {
        return false;
    }

    uintptr_t size;
    if (!mew_fs_get_size(path, &size)) return false;

    StringView sv = cstr_to_sv(path);
    ptrdiff_t i = sv_last_index_char(sv, '.');
    if (i != -1) {
        StringView ext = sv_slice_from(sv, i + 1);
        const char *content_type = NULL;

        if (sv_eq_cstr(ext, "css")) {
            content_type = "text/css";
        } else if (sv_eq_cstr(ext, "svg")) {
            content_type = "image/svg+xml";
        } else if (sv_eq_cstr(ext, "ico")) {
            content_type = "image/vnd.microsoft.icon";
        }

        if (content_type) {
            http_headermap_insert_cstrs(&response->headers, "Content-Type", content_type);
        }
    }

    http_response_body_set_sendfile(response, (ResponseSendFile) { path, size });

    return true;
}

