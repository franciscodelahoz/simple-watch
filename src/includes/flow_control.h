#ifndef FLOW_CONTROL_H
#define FLOW_CONTROL_H

#include <sys/select.h>

#include "executor.h"

typedef struct FlowState {
    bool exit_execution;
    bool pause_execution;
    struct timeval timeout;
    fd_set readfds;
    int pipe_fds[2];
} FlowState;

FlowState* init_flow_state();

int wait_for_event(FlowState* flow_state, int interval_in_seconds);

void free_flow_state(FlowState* flow_state);

#endif
