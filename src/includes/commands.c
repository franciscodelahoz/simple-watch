#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

char* get_command(int optind, const int argc, char** argv) {
    size_t command_length = 0;

    for (int i = optind; i < argc; i++) {
        command_length += strlen(argv[i]) + 1;
    }

    char* command = malloc(command_length);

    if (!command) {
        perror("malloc");
        return nullptr;
    }

    command[0] = '\0';

    for (int i = optind; i < argc; i++) {
        strcat(command, argv[i]);

        if (i < argc - 1) {
            strcat(command, " ");
        }
    }

    return command;
}

CommandOptions* parse_commands(const int argc, char** argv) {
    CommandOptions* options = malloc(sizeof(CommandOptions));

    if (options == nullptr) {
        printf("Failed to allocate memory for CommandOptions");
        return nullptr;
    }

    options->interval = DEFAULT_INTERVAL;
    options->color = false;
    options->no_color = false;
    options->help = false;
    options->missing_argument = false;
    options->command = nullptr;
    options->version = false;
    options->exit_on_error = false;

    if (argc == 1) {
        options->missing_argument = true;
        return options;
    }

    int opt;

    while ((opt = getopt_long(argc, argv, "+i:cnhve", long_options, NULL)) != -1) {
        switch (opt) {
            case 'i': {
                options->interval = atoi(optarg);

                if (options->interval <= 0) {
                    printf("Interval must be a positive integer\n");
                    return nullptr;
                }

                break;
            };

            case 'c': {
                options->color = true;
                break;
            };

            case 'n': {
                options->no_color = true;
                break;
            };

            case 'h': {
                options->help = true;
                break;
            };

            case 'v': {
                options->version = true;
                break;
            }

            case 'e': {
                options->exit_on_error = true;
                break;
            }

            default: {
                break;
            };
        }
    }

    if (optind >= argc) {
        options->missing_argument = true;
        return options;
    }

    options->command = get_command(optind, argc, argv);
    return options;
}

void print_usage(FILE* out, char** argv) {
    fprintf(out, "Usage: %s [options] command\n\n", argv[0]);
    fputs("Options:\n", out);
    fputs("  -i, --interval <secs>   Set the interval in seconds between updates\n", out);
    fputs("  -c, --color             Enable color output\n", out);
    fputs("  -C, --no-color          Disable color output\n", out);
    fputs("  -e, --exit-on-error     Exit if the exist status of the command was an error\n", out);
    fputs("  -h, --help              Show this help message\n", out);
    fputs("  -v, --version           Output version information and exit\n", out);

    exit(out == stderr ? 1 : EXIT_SUCCESS);
}

void print_version(FILE* out, char** argv) {
    fprintf(out, "%s - Version 0.1.0\n", argv[0]);
}

void free_commands_options(CommandOptions* options) {
    if (options == nullptr) {
        return;
    }

    if (options->command != NULL) {
        free(options->command);
        options->command = nullptr;
    }

    free(options);
    options = nullptr;
}
