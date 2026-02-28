#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "commands.h"
#include "terminal.h"

#define PAGE_HEIGHT_FALLBACK 50

#define LINE_BUFFER_FRAGMENT 4096

typedef struct ScrollBuffer {
    char** lines;
    int total_lines;
    int current_offset;
    char* command;
    int interval;
} Scroll_Buffer;

Scroll_Buffer* init_scroll_buffer(char* command, int interval);

void add_line(Scroll_Buffer* buffer, const char* line);

void display_page(const Scroll_Buffer* buffer);

void scroll_up(Scroll_Buffer* buffer);

void scroll_down(Scroll_Buffer* buffer);

void free_scroll_buffer(Scroll_Buffer* buffer);

int execute_command(Scroll_Buffer* buffer, const char* command);

void handle_command_exit_status(const Command_Options* options, int exit_command_status);

#endif
