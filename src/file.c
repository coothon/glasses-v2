#include "include/file.h"

bool is_folder(const char *folder_path) {
	struct stat sb;
	return ((stat(folder_path, &sb) == 0) && S_ISDIR(sb.st_mode));
}

bool is_file(const char *file_path) {
	// Check if this file exists. A return value of 0 means yes, so we not it.
	return !access(file_path, F_OK);
}

const char *get_filename() {

}

int count_files_in_dir(const char *dir_path) {
	DIR *dir = opendir(dir_path);
	if (!dir) return -1;

	int            file_count = 0;
	struct dirent *entry;

	while ((entry = readdir(dir)) != NULL)
		if (entry->d_type == DT_REG) ++file_count;

	closedir(dir);
	return file_count;
}

void get_filenames_in_dir(const char *dir_path, char ***outlist, int *outcount) {
	DIR *dir = opendir(dir_path);
	if (!dir) return;

	struct dirent *entry;

	*outcount = count_files_in_dir(dir_path);
	if (*outcount == -1) return;

	int pathlen = strlen(dir_path);

	// Allocate array to hold strings.
	*outlist = malloc((*outcount) * sizeof(char *)); 
	
	// Allocate strings.
	for (int i = 0; i < (*outcount); ++i) {
		// For whatever reason, d_name is max 256 chars.
		(*outlist)[i] = malloc((pathlen + 257) * sizeof(char)); 
	}

	int j = 0;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG) {
			// Must be copied, because d_name will soon be freed.
			strcpy((*outlist)[j], dir_path);
			strncat((*outlist)[j++], entry->d_name, 257);
		}
	}

	closedir(dir);
}


