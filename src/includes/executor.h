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
} ScrollBuffer;

ScrollBuffer* init_scroll_buffer(char* command, int interval);

void add_line(ScrollBuffer* buffer, const char* line);

void display_page(const ScrollBuffer* buffer);

void scroll_up(ScrollBuffer* buffer);

void scroll_down(ScrollBuffer* buffer);

void free_scroll_buffer(ScrollBuffer* buffer);

int execute_command(ScrollBuffer* buffer, const char* command);

void handle_command_exit_status(const CommandOptions* options, int exit_command_status);

#endif
