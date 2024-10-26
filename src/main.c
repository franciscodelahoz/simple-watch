#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>

#include "commands.h"
#include "terminal.h"
#include "executor.h"
#include "flow_control.h"
#include "signal_handler.h"

typedef struct {
    ScrollBuffer* executor;
    FlowState* flow_state;
    int write_fd;
} ScrollThreadArgs;

void* input_control_thread(void* args) {
    ScrollThreadArgs* scroll_args = (ScrollThreadArgs*) args;
    ScrollBuffer* executor = scroll_args->executor;

    int write_fd = scroll_args->write_fd;

    nodelay(stdscr, TRUE);

    while (true) {
        const int ch = getch();
        char signal;

        switch (ch) {
            case 'j': {
                scroll_down(executor);
                break;
            }

            case 'k': {
                scroll_up(executor);
                break;
            }

            case 'q': {
                signal = 'q';

                if (write(write_fd, &signal, 1) == -1) {
                    perror("Error writing the input signal to pipe");
                    exit_with_code(EXIT_FAILURE);
                }

                break;
            }

            case 'p': {
                signal = 'p';

                if (write(write_fd, &signal, 1) == -1) {
                    perror("Error writing the input signal to pipe");
                    exit_with_code(EXIT_FAILURE);
                }

                break;
            }

            default: {
                break;
            }

        }

        usleep(10000);
    }
}

int main(int argc, char** argv) {
    CommandOptions* options = parse_commands(argc, argv);

    if (options == nullptr) {
        printf("Failed to parse command options\n");
        exit_with_code(EXIT_FAILURE);
    }

    if (options->version) {
        print_version(stdout, argv);
        free_commands_options(options);
        exit(EXIT_SUCCESS);
    }

    if (options->help || options->missing_argument) {
        print_usage(stdout, argv);
        free_commands_options(options);
        exit(EXIT_SUCCESS);
    }

    register_signal_handlers(SIGINT, handle_stop_signal);
    register_signal_handlers(SIGHUP, handle_stop_signal);
    register_signal_handlers(SIGTERM, handle_stop_signal);

    Terminal* terminal = init_terminal();
    ScrollBuffer* executor = init_scroll_buffer(options->command, options->interval);

    configure_terminal();

    if (options->command == NULL) {
        printf("No command provided\n");
        exit_with_code(EXIT_FAILURE);
    }

    FlowState* flow_state = init_flow_state();
    int exit_status = 0;

    ScrollThreadArgs scroll_args = { executor, flow_state, flow_state->pipe_fds[1] };
    pthread_t scroll_thread;

    if (pthread_create(&scroll_thread, nullptr, input_control_thread, &scroll_args) != 0) {
        perror("Error creating scroll thread");
        exit(EXIT_FAILURE);
    }

    while (true) {
        if (!flow_state->pause_execution) {
            clear();
            refresh();

            exit_status = execute_command(executor, options->command);
            handle_command_exit_status(options, exit_status);
        }

        int signal = wait_for_event(flow_state, options->interval);

        if (signal == -1) {
            exit_with_code(EXIT_FAILURE);
            break;
        }

        if (signal > 0) {
            char buffer[1];
            read(flow_state->pipe_fds[0], buffer, 1);

            if (buffer[0] == 'q') {
                break;
            }

            if (buffer[0] == 'p') {
                flow_state->pause_execution = !flow_state->pause_execution;
            }
        }
    }

    pthread_cancel(scroll_thread);
    pthread_join(scroll_thread, nullptr);

    free_commands_options(options);
    free_flow_state(flow_state);
    free_terminal(terminal);
    free_scroll_buffer(executor);

    exit_with_code(EXIT_SUCCESS);
}
