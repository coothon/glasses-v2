#include "include/file.h"

bool is_file(const char *file_path) {
	// Check if this file exists. A return value of 0 means yes, so we not
	// it.
	return !access(file_path, F_OK);
}
