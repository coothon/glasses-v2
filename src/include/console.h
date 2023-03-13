#ifndef GLASSES2_CONSOLE_H_
#define GLASSES2_CONSOLE_H_
// Console I/O.

#include <stdarg.h>
#include <stdio.h>

void print_help(void);
void print_keybinds(void);
void print_error(const char *fmt, ...);
void print_info(const char *fmt, ...);
void print_debug(const char *fmt, ...);

#endif // GLASSES2_CONSOLE_H_
