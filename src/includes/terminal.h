#ifndef TERMINAL_H
#define TERMINAL_H

#define TERMINAL_HEIGHT_FALLBACK 24
#define TERMINAL_WIDTH_FALLBACK 80

typedef struct Terminal {
    unsigned int width;
    unsigned int height;
} Terminal;

Terminal* init_terminal(void);

void configure_terminal(void);

void exit_with_code(int code);

void gracefully_exit(void);

void print_message(int x, int y, const char *message);

void get_terminal_size(Terminal* terminal);

void free_terminal(Terminal* terminal);

#endif
