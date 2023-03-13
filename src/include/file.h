#ifndef GLASSES2_FILE_H_
#define GLASSES2_FILE_H_

#define _DEFAULT_SOURCE 1
#include <dirent.h> // This kills any possible Windows support, but it's just too damn useful.
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

bool        is_folder(const char *folder_path);
bool        is_file(const char *file_path);
int         count_files_in_dir(const char *dir_path);
void get_filenames_in_dir(const char *dir_path, char ***outlist, int *outcount); 

#endif // GLASSES2_FILE_H_
