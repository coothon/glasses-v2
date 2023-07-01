#ifndef CONSOLE_LOG_H_
#define CONSOLE_LOG_H_

#include <stdarg.h>
#include <stdio.h>

extern void con_log(const char *restrict head, const char *restrict tail, const char *restrict fmt, ...);

extern void con_log_loc(const char *restrict head, const char *restrict tail, const char *restrict file, unsigned line,
                        const char *restrict fmt, ...);

#ifndef CONSOLE_LOG_DEBUG
#define con_log_debug(...)
#endif /* CONSOLE_LOG_DEBUG */

#ifdef CONSOLE_LOG_INCLUDE_LOCATION
#define con_log_info(...) con_log_loc("", "", __FILE__, __LINE__, __VA_ARGS__)
#define con_log_warn(...) con_log_loc("WARNING: ", "", __FILE__, __LINE__, __VA_ARGS__)
#define con_log_error(...) con_log_loc("ERROR: ", "", __FILE__, __LINE__, __VA_ARGS__)
#define con_log_fatal(...) con_log_loc("FATAL: ", "", __FILE__, __LINE__, __VA_ARGS__)

#ifdef CONSOLE_LOG_DEBUG
#define con_log_debug(...) con_log_loc("DEBUG: ", "", __FILE__, __LINE__, __VA_ARGS__)
#endif /* CONSOLE_LOG_DEBUG */

#else
#define con_log_info(...) con_log("", "", __VA_ARGS__)
#define con_log_warn(...) con_log("WARNING: ", "", __VA_ARGS__)
#define con_log_error(...) con_log("ERROR: ", "", __VA_ARGS__)
#define con_log_fatal(...) con_log("FATAL: ", "", __VA_ARGS__)

#ifdef CONSOLE_LOG_DEBUG
#define con_log_debug(...) con_log("DEBUG: ", "", __VA_ARGS__)
#endif /* CONSOLE_LOG_DEBUG */

#endif /* CONSOLE_LOG_INCLUDE_LOCATION */

#endif /* CONSOLE_LOG_H_ */
