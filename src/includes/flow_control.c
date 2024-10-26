#include "flow_control.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

FlowState* init_flow_state() {
    FlowState* flow_control = malloc(sizeof(FlowState));

    if (flow_control == NULL) {
        perror("malloc");
        return nullptr;
    }

    if (pipe(flow_control->pipe_fds) == -1) {
        perror("Error creating pipe");
        free(flow_control);
        return nullptr;
    }

    flow_control->exit_execution = false;
    flow_control->pause_execution = false;
    flow_control->timeout = (struct timeval){ 0 };

    return flow_control;
}

int wait_for_event(FlowState* flow_control, int interval_in_seconds) {
    FD_ZERO(&flow_control->readfds);
    FD_SET(flow_control->pipe_fds[0], &flow_control->readfds);

    flow_control->timeout.tv_sec = interval_in_seconds;
    flow_control->timeout.tv_usec = 0;

    int result = select(flow_control->pipe_fds[0] + 1, &flow_control->readfds, nullptr, nullptr, &flow_control->timeout);

    if (result == -1) {
        perror("select error");
    }

    return result;
}

void free_flow_state(FlowState* flow_control) {
    close(flow_control->pipe_fds[0]);
    close(flow_control->pipe_fds[1]);
    free(flow_control);
}
