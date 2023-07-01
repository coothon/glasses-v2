#include "include/console_log.h"

void con_log(const char *restrict head, const char *restrict tail, const char *restrict fmt, ...) {
	fputs(head, stderr);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "%s\n", tail);
}

extern void con_log_loc(const char *restrict head, const char *restrict tail, const char *restrict file, unsigned line,
                        const char *restrict fmt, ...) {
	fprintf(stderr, "[%s:%u] %s", file, line, head);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "%s\n", tail);
}
