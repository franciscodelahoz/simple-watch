#ifndef COMMAND_H
#define COMMAND_H

#include <getopt.h>
#include <stdio.h>

#define DEFAULT_INTERVAL 2

typedef struct CommandOptions {
    int interval;
    bool color;
    bool no_color;
    bool help;
    bool missing_argument;
    bool version;
    bool exit_on_error;
    char* command;
} CommandOptions;

static struct option long_options[] = {
    {"interval", required_argument, 0, 'i'},
    {"color", no_argument, 0, 'c'},
    {"no-color", no_argument, 0, 'n'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {"exit-on-error", no_argument, 0, 'e'},
    {0, 0, 0, 0}
};

CommandOptions* parse_commands(int argc, char** argv);

void print_usage(FILE *out, char** argv);

void print_version(FILE* out, char** argv);

void free_commands_options(CommandOptions* options);

#endif
