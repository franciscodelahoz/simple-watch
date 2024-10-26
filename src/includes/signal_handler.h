#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

void handle_stop_signal(int signal);

void register_signal_handlers(int signal, void (*handler)(int));

void handle_resize_signal(int signal);

bool should_handle_resize(void);

void set_handle_resize(bool value);

#endif
