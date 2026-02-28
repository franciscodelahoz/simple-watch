#ifndef FLOW_CONTROL_H
#define FLOW_CONTROL_H

#include <sys/select.h>

#include "executor.h"

typedef struct Flow_State {
    bool exit_execution;
    bool pause_execution;
    struct timeval timeout;
    fd_set readfds;
    int pipe_fds[2];
} Flow_State;

Flow_State* init_flow_state();

int wait_for_event(Flow_State* flow_state, int interval_in_seconds);

void free_flow_state(Flow_State* flow_state);

#endif
