#include "mew/utils.h"

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mew/log.h"

bool read_file_to_sb(const char *path, StringBuilder *asb) {
    bool result = true;
    int fd = open(path, O_RDONLY);
    if (fd < 0) return_defer(false);
    struct stat stat_buf;
    if (stat(path, &stat_buf) < 0) return_defer(false);

    char buf[1024] = {0};
    while ((long) asb->count < stat_buf.st_size) {
        ssize_t size = read(fd, buf, sizeof(buf));
        if (size < 0) return_defer(false);
        sb_append_buf(asb, buf, (size_t) size);
    }

defer:
    if (fd > 0) close(fd);
    return result;
}

volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) {
    (void) sig;
    interrupted = 1;
}

void wait_for_ctrl_c() {
    struct sigaction sa = {0};
    sa.sa_handler = handle_sigint;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        log_error("Could not set sigaction: %s", strerror(errno));
        exit(1);
    }

    while (!interrupted) {
        pause();
    }
}

