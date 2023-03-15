#define _DEFAULT_SOURCE 1
#include "include/glad.h"
#include "include/khrplatform.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include "include/common.h"
#include "include/conf.h"
#include "include/console.h"
#include "include/file.h"
#include "include/list.h"
#include "include/render.h"

int         switch_sample  = SWITCH_LANCZOS;
bool        many_file_mode = false;
const char *path_to_image  = NULL;
glasses     prog           = { 0 };

static int switch_long(const char *long_switch) {
	if (strcmp(long_switch, "--help") == 0) {
		print_help();
		exit(0);
	} else if (strcmp(long_switch, "--keybinds") == 0) {
		print_keybinds();
		exit(0);
	} else if (strcmp(long_switch, "--bilinear") == 0) {
		switch_sample = SWITCH_BILINEAR;
		return 0;
	} else if (strcmp(long_switch, "--nearest") == 0) {
		switch_sample = SWITCH_NEAREST;
		return 0;
	} else if (strcmp(long_switch, "--lanczos") == 0) {
		switch_sample = SWITCH_LANCZOS;
		return 0;
	}

	return -1;
}

static int switch_short(const char *switches) {
	int len = strlen(switches);
	for (int i = 1; i < len; ++i) {
		switch (switches[i]) {
		case 'h': {
			switch_sample = SWITCH_LANCZOS;
		} break;

		case 'n': {
			switch_sample = SWITCH_NEAREST;
		} break;

		case 'l': {
			switch_sample = SWITCH_BILINEAR;
		} break;

		default:
			return -1;
		}
	}

	return 0;
}

void clean_up(void) {
	glfwDestroyWindow(prog.win);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		print_info(
		    "No file path supplied. Defaulting to `./' directory.");
		many_file_mode      = true;
		prog.many_files_dir = "./";
	}

	stbi_set_flip_vertically_on_load(1);
	bool found_image = false;
	bool stop_opt    = false;

	// Handle arguments.
	for (int i = 1; i < argc; ++i) {
		if (!stop_opt && strcmp(argv[i], "--") == 0) {
			// dash dash stops option parsing
			stop_opt = true;
		} else if (!stop_opt && argv[i][0] == '-' &&
		           argv[i][1] != '\0' && argv[i][1] != '-') {
			// argv[i] is a short switch.
			if (switch_short(argv[i]) < 0) {
				print_error("Invalid argument: `%s.'", argv[i]);
				return EXIT_FAILURE;
			}
		} else if (!stop_opt && argv[i][0] == '-' &&
		           argv[i][1] == '-') {
			// argv[i] is a long switch.
			if (switch_long(argv[i]) < 0) {
				print_error("Invalid argument: `%s.'", argv[i]);
				return EXIT_FAILURE;
			}
		} else if (is_folder(argv[i]) && !found_image) {
			many_file_mode      = true;
			prog.many_files_dir = argv[i];
			found_image         = true;
		} else if (is_file(argv[i]) && !found_image &&
		           !many_file_mode) {
			// argv[i] is the path to the image.
			path_to_image = argv[i];
			found_image   = true;
		} else if (is_file(argv[i]) && found_image && !many_file_mode) {
			print_info("Ignoring additional file path: `%s.'",
			           argv[i]);
		} else {
			print_error("Invalid argument: `%s.'", argv[i]);
			return EXIT_FAILURE;
		}
	}


	if (!many_file_mode) {
		// Get user-supplied image from disk.
		prog.current_image = path_to_image;
		prog.imgdata =
		    stbi_load(path_to_image, &prog.imgwidth, &prog.imgheight,
		              &prog.imgchannels, STBI_rgb_alpha);
		if (!prog.imgdata) {
			print_error(
			    "Unable to load image from file `%s,' or the file is not a supported format (see `--help').",
			    path_to_image);
			return EXIT_FAILURE;
		}
		prog.imgwidth_f  = (GLfloat)prog.imgwidth;
		prog.imgheight_f = (GLfloat)prog.imgheight;
	} else {
		int dirstrlen = strlen(prog.many_files_dir);
		if (prog.many_files_dir[dirstrlen - 1] != '/') {
			// If there is no trailing slash, append one.
			char  slash[] = "/";
			char *tmp     = malloc(dirstrlen + sizeof slash);
			char *p = mem_copy(tmp, prog.many_files_dir, dirstrlen);
			p       = mem_copy(p, slash,
			                   sizeof slash); /* also copies the nul-byte
			                                     from `slash` */
			prog.many_files_dir = tmp;
		}
		prog.many_files = get_filenames_in_dir(prog.many_files_dir);
		if (!prog.many_files) exit(EXIT_FAILURE);
		// Try load first file.
		if (!many_files_load(prog.many_files)) {
			// Keep trying until a valid image is found; otherwise,
			// exit.
			if (many_files_next(&prog) != 0) {
				print_error("No image files in directory.");
				exit(EXIT_FAILURE);
			}
		}
		prog.many_files_total_count = count_nodes(prog.many_files);
		prog.many_files_current_index =
		    get_index_from_beginning(prog.many_files);
	}

	// GLFW init.
	if (!glfwInit()) {
		print_error("GLFW initialization failed.");
		return EXIT_FAILURE;
	}

	glfwSetErrorCallback(glfw_error_cb);

	// Window config.
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	prog.win = glfwCreateWindow(1280, 720, "Glasses", NULL, NULL);
	if (!prog.win) {
		print_error("GLFW window creation failed.");
		return EXIT_FAILURE;
	}

	atexit(clean_up);

	glfwMakeContextCurrent(prog.win);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwGetFramebufferSize(prog.win, &prog.window_size[0],
	                       &prog.window_size[1]);
	glViewport(0, 0, prog.window_size[0], prog.window_size[1]);

	glfwSetFramebufferSizeCallback(prog.win, glfw_framebuffer_size_cb);
	glfwSetKeyCallback(prog.win, glfw_key_cb);
	glfwSetCursorPosCallback(prog.win, glfw_cursorpos_cb);
	glfwSetMouseButtonCallback(prog.win, glfw_click_cb);
	glfwSetScrollCallback(prog.win, glfw_scroll_cb);

	glfwSwapInterval(0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	renderer_init(&prog);

	// Main event loop.
	while (!glfwWindowShouldClose(prog.win)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1f(prog.image.uniform_scale, prog.image.scale);
		glUniform2f(prog.image.uniform_logical_pos,
		            prog.image.logical_pos[0],
		            prog.image.logical_pos[1]);
		glDrawElements(GL_TRIANGLES, prog.image.num_indices,
		               GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(prog.win);
		glfwWaitEvents();
	}

	return EXIT_SUCCESS;
}
