#include "terminal.h"

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

Terminal* init_terminal(void) {
    Terminal* terminal = malloc(sizeof(Terminal));

    if (terminal == NULL) {
        return nullptr;
    }

    get_terminal_size(terminal);

    return terminal;
}

void configure_terminal(void) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
}

void exit_with_code(const int code) {
    endwin();
    exit(code);
}

void gracefully_exit(void) {
   exit_with_code(EXIT_SUCCESS);
}

void print_message(const int x, const int y, const char *message) {
    clear();
    mvprintw(x, y, "%s", message);
    refresh();
}

void get_terminal_size(Terminal* terminal) {
    struct winsize w;

    if (ioctl(STDERR_FILENO, TIOCGWINSZ, &w) == 0) {
        terminal->width = w.ws_col;
        terminal->height = w.ws_row;
    } else {
        const char* columns_env = getenv("COLUMNS");
        const char* lines_env = getenv("LINES");

        if (columns_env != NULL && lines_env != NULL) {
            terminal->width = atoi(columns_env);
            terminal->height = atoi(lines_env);
        }
    }

    if (terminal->width == 0 || terminal->height == 0) {
        terminal->height = TERMINAL_HEIGHT_FALLBACK;
        terminal->width = TERMINAL_WIDTH_FALLBACK;
    }
}

void free_terminal(Terminal* terminal) {
    free(terminal);
}
