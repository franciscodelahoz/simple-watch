#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "executor.h"
#include "terminal.h"

ScrollBuffer* init_scroll_buffer(char* command, int interval) {
    ScrollBuffer* buffer = malloc(sizeof(ScrollBuffer));

    buffer->lines = nullptr;
    buffer->total_lines = 0;
    buffer->current_offset = 0;
    buffer->command = command;
    buffer->interval = interval;

    return buffer;
}

void add_line(ScrollBuffer* buffer, const char* line) {
    buffer->lines = realloc(buffer->lines, sizeof(char*) * (buffer->total_lines + 1));

    if (buffer->lines == nullptr) {
        fprintf(stderr, "Failed to allocate memory for line buffer\n");
        exit(EXIT_FAILURE);
    }

    buffer->lines[buffer->total_lines] = strdup(line);
    buffer->total_lines++;
}

void display_header(const char* command, const int interval) {
    const time_t now = time(nullptr);

    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    attron(COLOR_PAIR(2) | A_REVERSE);

    mvprintw(0, 0, "Every %d.0s: %s    %s", interval, command, time_str);

    clrtoeol();
    attroff(COLOR_PAIR(2) | A_REVERSE);
}

void display_page(const ScrollBuffer* buffer) {
    clear();

    display_header(buffer->command, buffer->interval);

    constexpr int start_row = 2;
    const int max_rows = LINES - start_row;
    const int total_lines = buffer->total_lines;

    const int start = buffer->current_offset;
    int end = start + max_rows;

    if (end > total_lines) {
        end = total_lines;
    }

    if (buffer->lines != NULL) {
        for (int i = start; i < end; i++) {
            mvprintw(start_row + i - start, 0, "%s", buffer->lines[i]);
        }
    }

    refresh();
}

void scroll_up(ScrollBuffer* buffer) {
    if (buffer->current_offset > 0) {
        buffer->current_offset--;
        display_page(buffer);
    }
}

void scroll_down(ScrollBuffer* buffer) {
    if (buffer->current_offset + PAGE_HEIGHT_FALLBACK < buffer->total_lines) {
        buffer->current_offset++;
        display_page(buffer);
    }
}

void free_scroll_buffer(ScrollBuffer* buffer) {
    for (int i = 0; i < buffer->total_lines; i++) {
        free(buffer->lines[i]);
    }

    free(buffer->lines);
    free(buffer);
}

void clear_command_executor(ScrollBuffer* buffer) {
    for (int i = 0; i < buffer->total_lines; i++) {
        free(buffer->lines[i]);
    }

    free(buffer->lines);

    buffer->lines = nullptr;
    buffer->total_lines = 0;
    buffer->current_offset = 0;
}

void normalize_offset(ScrollBuffer* buffer) {
    if (buffer->current_offset > buffer->total_lines - PAGE_HEIGHT_FALLBACK) {
        buffer->current_offset = buffer->total_lines > PAGE_HEIGHT_FALLBACK ?
            buffer->total_lines - PAGE_HEIGHT_FALLBACK : 0;
    }
}

void strip_ansi(char* str) {
    const char* src = str;
    char* dst = str;

    while (*src) {
        if (*src == '\033') {
            src++;

            if (*src == '[') {
                src++;

                while (*src && !isalpha(*src)) {
                    src++;
                }

                if (*src) {
                    src++;
                }
            }
        } else {
            *dst++ = *src++;
        }
    }

    *dst = '\0';
}

char* dynamic_strcat(char* dest, const char* src) {
    if (dest == nullptr) {
        dest = strdup(src);
    } else {
        const size_t dest_len = strlen(dest);
        const size_t src_len = strlen(src);

        dest = realloc(dest, dest_len + src_len + 1);

        if (dest == nullptr) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        strcat(dest, src);
    }

    return dest;
}

int execute_command(ScrollBuffer* buffer, const char* command) {
    clear_command_executor(buffer);

    int pipe_fd[2];
    int status;

    if (pipe(pipe_fd) == -1) {
        perror("Error creating pipe");
        return -1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Error during fork");
        return -1;
    }

    if (pid == 0) {
        close(pipe_fd[0]);

        dup2(pipe_fd[1], STDOUT_FILENO);
        dup2(pipe_fd[1], STDERR_FILENO);

        close(pipe_fd[1]);

        status = system(buffer->command);

        if (status == -1) {
            (void) !write(STDERR_FILENO, buffer->command, sizeof(buffer->command) - 1);
            (void) !write(STDERR_FILENO, ": unable to run", 15);
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            exit(WEXITSTATUS(status));
        }

        exit(EXIT_SUCCESS);
    }

    /* If pid is not 0, then it is the parent process */
    close(pipe_fd[1]);

    FILE* fp = fdopen(pipe_fd[0], "r");

    if (fp == NULL) {
        perror("Error opening pipe");
        close(pipe_fd[0]);
        return -1;
    }

    setvbuf(fp, nullptr, _IOFBF, BUFSIZ);

    char* line_buffer = nullptr;
    char fragment[LINE_BUFFER_FRAGMENT];

    while (fgets(fragment, sizeof(fragment), fp) != NULL) {
        line_buffer = dynamic_strcat(line_buffer, fragment);

        if (strchr(fragment, '\n')) {
            strip_ansi(line_buffer);
            add_line(buffer, line_buffer);
            free(line_buffer);
            line_buffer = nullptr;
        }
    }

    if (line_buffer != nullptr) {
        strip_ansi(line_buffer);
        add_line(buffer, line_buffer);
        free(line_buffer);
    }

    fclose(fp);

    normalize_offset(buffer);
    display_page(buffer);

    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    if (WIFSIGNALED(status)) {
        return WTERMSIG(status);
    }

    return 0;
}

void handle_command_exit_status(const CommandOptions* options, int exit_command_status) {
    if (exit_command_status != 0 && options->exit_on_error) {
        const char* message = "Command exit with a non-zero status. Press any key to exit...";
        print_message(0, 0, message);

        getchar();
        exit_with_code(exit_command_status);
    }
}
