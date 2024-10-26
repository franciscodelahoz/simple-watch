#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "terminal.h"

static volatile bool handle_resize = false;

void handle_stop_signal(int signal __attribute__((unused))) {
    gracefully_exit();
}

void handle_resize_signal(int signal __attribute__((unused))) {
    handle_resize = true;
}

bool should_handle_resize(void) {
    return handle_resize;
}

void set_handle_resize(bool value) {
    handle_resize = value;
}

void register_signal_handlers(int signal, void (*handler)(int)) {
    struct sigaction sa;

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;

    if (sigaction(signal, &sa, NULL) == -1) {
        perror("Error while setting signal handler");
        exit(EXIT_FAILURE);
    }
}
