#ifndef GLASSES2_CONF_H_
#define GLASSES2_CONF_H_

#include <stdbool.h>

#define SWITCH_NEAREST 0
#define SWITCH_BILINEAR 1
#define SWITCH_LANCZOS 2

extern int switch_sample;
extern bool many_file_mode;
extern const char *path_to_image;

#endif // GLASSES2_CONF_H_
