#include "include/console.h"

void print_help(void) {
	puts("Glasses:");
	puts(
		"  This is a program for viewing images \"interactively\": click and drag");
	puts(
		"  to move the image, and scroll to zoom. See --keybinds for more. Should");
	puts(
		"  support whatever images stb_image.h supports, as long as it can be");
	puts("  converted to RGBA.");
	puts(
		"    The program uses OpenGL hardware acceleration, thus it will not work");
	puts(
		"  without a video card that supports OpenGL and appopriate drivers. If you");
	puts("  are unsure whether you have that, you probably do.");
	puts("");
	puts("Usage:");
	puts("  `glasses [switches] [image_path.{png,jpg,jpeg}]'");
	puts("");
	puts("Switches:");
	puts("  [short]");
	puts("  `-l': shorthand for `--bilinear'");
	puts("  `-n': shorthand for `--nearest'");
	puts("  `-h': shorthand for `--lanczos'");
	puts("  [long]");
	puts("  `--bilinear': start with bilinear sampling");
	puts("  `--nearest':  start with nearest neighbour sampling");
	puts("  `--lanczos':  start with lanczos sampling");
	puts("  `--help':     print the help message");
	puts("  `--keybinds': print the keybinds");
	puts("");
	print_keybinds();
}

void print_keybinds(void) {
	puts("Keybinds:");
	puts("  Left click and drag:  move image");
	puts("  Scroll wheel/`+'/`-': zoom in and out");
	puts("  `r':                  reset position and zoom");
	puts("  `n':                  set sampling mode to nearest neighbour");
	puts("  `l':                  set sampling mode to bilinear");
	puts("  `h':                  set sampling mode to lanczos");
	puts("  ESC/`q':              quit");
	puts("");
}

void print_error(const char *fmt, ...) {
	va_list args;

	fprintf(stderr, "[ERROR]: ");

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "\n");
}

void print_info(const char *fmt, ...) {
	va_list args;

	printf("[INFO]: ");

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	printf("\n");
}

void print_debug(const char *fmt, ...) {
	va_list args;

	printf("[DEBUG]: ");

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	printf("\n");
}
