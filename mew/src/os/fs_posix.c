#include "mew/os/fs.h"

#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mew/utils.h"

bool mew_fs_exists(const char *path) {
    struct stat s;
    return stat(path, &s) == 0;
}

bool mew_fs_get_size(const char *path, uintptr_t *size) {
    struct stat s;
    if (stat(path, &s) != 0) return false;
    if (size != NULL) *size = s.st_size;
    return true;
}

bool mew_fs_read_file_to_sb(const char *path, StringBuilder *asb) {
    bool result = true;
    int fd = open(path, O_RDONLY);
    if (fd < 0) return_defer(false);
    struct stat stat_buf;
    if (stat(path, &stat_buf) < 0) return_defer(false);

    char buf[1024] = {0};
    while ((long) asb->count < stat_buf.st_size) {
        ptrdiff_t size = read(fd, buf, sizeof(buf));
        if (size < 0) return_defer(false);
        sb_append_buf(asb, buf, (size_t) size);
    }

defer:
    if (fd > 0) close(fd);
    return result;
}
