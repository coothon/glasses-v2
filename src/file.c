#include "include/file.h"
#include "include/list.h"

bool is_folder(const char *folder_path) {
	struct stat sb;
	return ((stat(folder_path, &sb) == 0) && S_ISDIR(sb.st_mode));
}

bool is_file(const char *file_path) {
	// Check if this file exists. A return value of 0 means yes, so we not
	// it.
	return !access(file_path, F_OK);
}

const char *get_filename() {}

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

/* portable mempcpy, written by u/N-R-K on Reddit */
void *mem_copy(void *restrict dst, const void *restrict src, size_t n) {
	return memcpy(dst, src, n) + n;
}

list_node *get_filenames_in_dir(const char *dir_path) {
	DIR *dir = opendir(dir_path);
	if (!dir) return NULL;

	struct dirent *entry;

	int pathlen = strlen(dir_path);

	// Set up first node; make it current; save for returning later.
	list_node *current_node = malloc(sizeof(list_node));
	if (!current_node) return NULL;
	current_node->prev   = NULL;
	list_node *next_node = NULL;
	list_node *beginning = current_node;

	int count = 0;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG) {
			size_t d_name_len  = strlen(entry->d_name);
			current_node->item = malloc(pathlen + d_name_len + 1);
			if (!current_node->item) return NULL;

			// I think I'm doing this right.
			*((char *)mem_copy(mem_copy(current_node->item, dir_path, pathlen),
			                   entry->d_name, d_name_len)) = '\0';

			// Create next node.
			next_node    = insert_after(current_node, NULL);
			current_node = next_node;
			++count;
		}
	}


	if (current_node->prev) current_node->prev->next = NULL;
	free(current_node); // Last one is always unused.

	closedir(dir);
	if (count == 0) return NULL;
	return beginning;
}
