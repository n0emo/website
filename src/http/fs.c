#include "http/fs.h"

#include <sys/stat.h>

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

    struct stat file_stat = {0};
    if (stat(path, &file_stat) != 0) return false;

    response->body.kind = RESPONSE_BODY_SENDFILE;
    response->body.as.sendfile = (ResponseSendFile) {
        .path = path,
        .size = file_stat.st_size,
    };

    return true;
}

