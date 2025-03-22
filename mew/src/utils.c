#include "mew/utils.h"

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: port to Windows
#include <unistd.h>

#include "mew/log.h"

volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) {
    (void) sig;
    interrupted = 1;
}

void wait_for_ctrl_c(void) {
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

