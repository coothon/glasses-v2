#include "include/render.h"
#include "include/embedded_shaders.h"

GLint renderer_init(glasses *g) {
	g->image.vertices     = rect_vertices;
	g->image.num_vertices = NUM_RECT_VERTICES;

	g->image.indices     = rect_indices;
	g->image.num_indices = NUM_RECT_INDICES;

	renderer *r = &g->image;

	r->scale          = 1.0f;
	r->logical_pos[0] = 0.0f;
	r->logical_pos[1] = 0.0f;

	glGenVertexArrays(1, &r->VAO);
	glGenBuffers(1, &r->VBO);
	glGenBuffers(1, &r->EBO);
	glBindVertexArray(r->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, r->VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * r->num_vertices, r->vertices,
	             GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * r->num_indices,
	             r->indices, GL_STATIC_DRAW);

	r->vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(r->vert_shader, 1, &embedded_shader_image_vert, NULL);
	glCompileShader(r->vert_shader);

	/* Check. */ {
		GLint  shader_status;
		GLchar shader_log[512];
		glGetShaderiv(r->vert_shader, GL_COMPILE_STATUS, &shader_status);
		if (!shader_status) {
			glGetShaderInfoLog(r->vert_shader, 512, NULL, shader_log);
			print_error(shader_log);
			exit(EXIT_FAILURE);
		}
	}

	r->frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(r->frag_shader, 1, &embedded_shader_image_frag, NULL);
	glCompileShader(r->frag_shader);

	/* Check. */ {
		GLint  shader_status;
		GLchar shader_log[512];
		glGetShaderiv(r->frag_shader, GL_COMPILE_STATUS, &shader_status);
		if (!shader_status) {
			glGetShaderInfoLog(r->frag_shader, 512, NULL, shader_log);
			print_error(shader_log);
			exit(EXIT_FAILURE);
		}
	}

	r->shader_program = glCreateProgram();
	glAttachShader(r->shader_program, r->vert_shader);
	glAttachShader(r->shader_program, r->frag_shader);
	glLinkProgram(r->shader_program);

	/* Check. */ {
		GLint  program_status;
		GLchar program_log[512];
		glGetProgramiv(r->shader_program, GL_LINK_STATUS, &program_status);
		if (!program_status) {
			glGetProgramInfoLog(r->shader_program, 512, NULL, program_log);
			print_error(program_log);
			exit(EXIT_FAILURE);
		}
	}
	glDeleteShader(r->vert_shader);
	glDeleteShader(r->frag_shader);
	glUseProgram(r->shader_program);

#if GLASSES2_DEBUG
	print_debug("Image shaders compiled; program linked successfully.");
#endif

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
	                      (void *)offsetof(vertex, uv));



	r->uniform_is_lanczos =
		glGetUniformLocation(r->shader_program, "is_lanczos");
	r->uniform_texsize = glGetUniformLocation(r->shader_program, "tex_size");
	r->uniform_scale   = glGetUniformLocation(r->shader_program, "scale");
	r->uniform_perfect_scale =
		glGetUniformLocation(r->shader_program, "perfect_scale");
	r->uniform_logical_pos =
		glGetUniformLocation(r->shader_program, "logical_pos");
	r->uniform_viewport_size =
		glGetUniformLocation(r->shader_program, "viewport_size");

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(r->shader_program, "tex"), 0);
	glGenTextures(1, &r->texture);
	glBindTexture(GL_TEXTURE_2D, r->texture);

	renderer_send_image(g);

	switch (switch_sample) {
	case SWITCH_NEAREST: {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glUniform1i(g->image.uniform_is_lanczos, 0);
	} break;

	case SWITCH_BILINEAR: {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glUniform1i(g->image.uniform_is_lanczos, 0);
	} break;

	case SWITCH_LANCZOS: {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glUniform1i(g->image.uniform_is_lanczos, 1);
	} break;

#if GLASSES2_DEBUG
	default: {
		print_error("Unexpected occurrence (switch_sample = %d) at: %s:%s.",
		            switch_sample, __FILE__, __LINE__);
	}
		exit(EXIT_FAILURE);
#else
	default: {
		print_info(
			"Something unexpected but recoverable happened. Falling back to nearest neighbour sampling.");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glUniform1i(g->image.uniform_is_lanczos, 0);
	} break;
#endif
	}

	glUniform2f(g->image.uniform_viewport_size, (GLfloat)g->window_size[0],
	            (GLfloat)g->window_size[1]);
	glUniform1f(g->image.uniform_perfect_scale, 1.0f);

	return EXIT_SUCCESS;
}

GLint many_files_prev(glasses *g) {
	if (!g->many_files->prev) return -1;

	list_node *current_enum = g->many_files->prev;
	for (;;) {
		// If file is invalid.
		if (!many_files_load(current_enum)) {
			current_enum = current_enum->next;
			// Remove it, bringing the next ones up.
			remove_node_free(current_enum->prev, true);
		} else {
			// Found valid image.
			g->many_files             = current_enum;
			g->many_files_total_count = count_nodes(g->many_files);
			g->many_files_current_index =
				get_index_from_beginning(g->many_files);
			return 0;
		}

		// No more left.
		if (!current_enum->prev) return -1;
		current_enum = current_enum->prev;
	}
}

GLint many_files_next(glasses *g) {
	if (!g->many_files->next) return -1;

	list_node *current_enum = g->many_files->next;
	for (;;) {
		// If file is invalid.
		if (!many_files_load(current_enum)) {
			current_enum = current_enum->prev;
			// Remove it, bringing the next ones down.
			remove_node_free(current_enum->next, true);
		} else {
			// Found valid image.
			g->many_files             = current_enum;
			g->many_files_total_count = count_nodes(g->many_files);
			g->many_files_current_index =
				get_index_from_beginning(g->many_files);
			return 0;
		}

		// No more left.
		if (!current_enum->next) return -1;
		current_enum = current_enum->next;
	}
}

list_node *many_files_load(list_node *file_node) {
	if (!file_node) return NULL;
	if (!file_node->item) return NULL;

	prog.imgdata = stbi_load(file_node->item, &prog.imgwidth, &prog.imgheight,
	                         &prog.imgchannels, STBI_rgb_alpha);
	if (!prog.imgdata) return NULL;
	prog.imgwidth_f  = (GLfloat)prog.imgwidth;
	prog.imgheight_f = (GLfloat)prog.imgheight;
	return file_node;
}

GLint renderer_send_image(glasses *g) {
	renderer *r = &g->image;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g->imgwidth, g->imgheight, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, g->imgdata);
	stbi_image_free(g->imgdata);

	glUniform2f(g->image.uniform_texsize, g->imgwidth_f, g->imgheight_f);
}

void glfw_error_cb(GLint e, const GLchar *desc) {
	(void)e;
	print_error(desc);
}

void glfw_key_cb(GLFWwindow *w, GLint key, GLint scancode, GLint action,
                 GLint mods) {
	(void)scancode;
	(void)mods;


	switch (action) {
	case GLFW_PRESS: {
		switch (key) {
		case GLFW_KEY_H: {
			glUniform1i(prog.image.uniform_is_lanczos, 1);
		} break;

		case GLFW_KEY_L: {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glUniform1i(prog.image.uniform_is_lanczos, 0);
		} break;

		case GLFW_KEY_N: {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glUniform1i(prog.image.uniform_is_lanczos, 0);
		} break;

		case GLFW_KEY_R: {
			prog.drag_mode            = GL_FALSE;
			prog.image.logical_pos[0] = 0.0f;
			prog.image.logical_pos[1] = 0.0f;
			prog.image.scale          = 1.0f;
		} break;

		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: {
			glfwSetWindowShouldClose(w, GLFW_TRUE);
		} break;

		case GLFW_KEY_EQUAL: {
			glfw_scroll_cb(NULL, 0.0, 1.0);
		} break;
		case GLFW_KEY_MINUS: {
			glfw_scroll_cb(NULL, 0.0, -1.0);
		} break;

		case GLFW_KEY_UP: {
			prog.image.logical_pos[1] -= MOVE_AMOUNT;
		} break;

		case GLFW_KEY_DOWN: {
			prog.image.logical_pos[1] += MOVE_AMOUNT;
		} break;

		case GLFW_KEY_LEFT: {
			prog.image.logical_pos[0] += MOVE_AMOUNT;
		} break;

		case GLFW_KEY_RIGHT: {
			prog.image.logical_pos[0] -= MOVE_AMOUNT;
		} break;

		case GLFW_KEY_ENTER: {
			if (many_file_mode && many_files_next(&prog) == 0) {
				GLfloat perfect_scale = 0.0f;
				renderer_send_image(&prog);
				if (prog.window_size[0] < prog.window_size[1]) {
					perfect_scale =
						(prog.imgwidth_f > prog.imgheight_f)
							? prog.window_size[0] / prog.imgwidth_f
							: prog.window_size[0] / prog.imgheight_f;
				} else {
					perfect_scale =
						(prog.imgwidth_f > prog.imgheight_f)
							? prog.window_size[1] / prog.imgwidth_f
							: prog.window_size[1] / prog.imgheight_f;
				}
				prog.drag_mode            = GL_FALSE;
				prog.image.logical_pos[0] = 0.0f;
				prog.image.logical_pos[1] = 0.0f;
				prog.image.scale          = 1.0f;
				glUniform1f(prog.image.uniform_perfect_scale, perfect_scale);

				print_debug("Current Image: [%d/%d].\n",
				            prog.many_files_current_index,
				            prog.many_files_total_count);
			}
		} break;

		case GLFW_KEY_BACKSPACE: {
			if (many_file_mode && many_files_prev(&prog) == 0) {
				GLfloat perfect_scale = 0.0f;
				renderer_send_image(&prog);
				if (prog.window_size[0] < prog.window_size[1]) {
					perfect_scale =
						(prog.imgwidth_f > prog.imgheight_f)
							? prog.window_size[0] / prog.imgwidth_f
							: prog.window_size[0] / prog.imgheight_f;
				} else {
					perfect_scale =
						(prog.imgwidth_f > prog.imgheight_f)
							? prog.window_size[1] / prog.imgwidth_f
							: prog.window_size[1] / prog.imgheight_f;
				}
				prog.drag_mode            = GL_FALSE;
				prog.image.logical_pos[0] = 0.0f;
				prog.image.logical_pos[1] = 0.0f;
				prog.image.scale          = 1.0f;
				glUniform1f(prog.image.uniform_perfect_scale, perfect_scale);

				print_debug("Current Image: [%d/%d].\n",
				            prog.many_files_current_index,
				            prog.many_files_total_count);
			}
		} break;

		default:
			break;
		}
	} break;
	case GLFW_REPEAT: {
		switch (key) {
		case GLFW_KEY_EQUAL: {
			glfw_scroll_cb(NULL, 0.0, 1.0);
		} break;

		case GLFW_KEY_MINUS: {
			glfw_scroll_cb(NULL, 0.0, -1.0);
		} break;

		case GLFW_KEY_UP: {
			prog.image.logical_pos[1] -= MOVE_AMOUNT;
		} break;

		case GLFW_KEY_DOWN: {
			prog.image.logical_pos[1] += MOVE_AMOUNT;
		} break;

		case GLFW_KEY_LEFT: {
			prog.image.logical_pos[0] += MOVE_AMOUNT;
		} break;

		case GLFW_KEY_RIGHT: {
			prog.image.logical_pos[0] -= MOVE_AMOUNT;
		} break;

		default:
			break;
		}
	} break;
	default:
		break;
	}
}

void glfw_framebuffer_size_cb(GLFWwindow *w, GLint fbwidth, GLint fbheight) {
	(void)w;
	GLfloat perfect_scale = 0.0f;

	prog.window_size[0] = fbwidth;
	prog.window_size[1] = fbheight;
	glViewport(0, 0, fbwidth, fbheight);

	if (prog.window_size[0] < prog.window_size[1]) {
		perfect_scale = (prog.imgwidth_f > prog.imgheight_f)
		                    ? prog.window_size[0] / prog.imgwidth_f
		                    : prog.window_size[0] / prog.imgheight_f;
	} else {
		perfect_scale = (prog.imgwidth_f > prog.imgheight_f)
		                    ? prog.window_size[1] / prog.imgwidth_f
		                    : prog.window_size[1] / prog.imgheight_f;
	}

	glUniform2f(prog.image.uniform_viewport_size, (GLfloat)prog.window_size[0],
	            (GLfloat)prog.window_size[1]);
	glUniform1f(prog.image.uniform_perfect_scale, perfect_scale);

	prog.drag_mode            = GL_FALSE;
	prog.image.logical_pos[0] = 0.0f;
	prog.image.logical_pos[1] = 0.0f;
	prog.image.scale          = 1.0f;
}

void glfw_cursorpos_cb(GLFWwindow *w, GLdouble x, GLdouble y) {
	(void)w;
	prog.cursor[0] = x;
	prog.cursor[1] = y;

	if (prog.drag_mode) {
		prog.image.logical_pos[0] = GLFW_TO_SCREEN_X(x) - prog.drag_start[0];
		prog.image.logical_pos[1] = GLFW_TO_SCREEN_Y(y) - prog.drag_start[1];
	}
}

void glfw_click_cb(GLFWwindow *w, GLint button, GLint action, GLint mods) {
	(void)w;
	(void)mods;

	switch (button) {
	case GLFW_MOUSE_BUTTON_1: {
		switch (action) {
		case GLFW_PRESS: {
			prog.drag_start[0] =
				GLFW_TO_SCREEN_X(prog.cursor[0]) - prog.image.logical_pos[0];
			prog.drag_start[1] =
				GLFW_TO_SCREEN_Y(prog.cursor[1]) - prog.image.logical_pos[1];
			prog.drag_mode = GL_TRUE;
		} break;

		case GLFW_RELEASE: {
			prog.drag_mode = GL_FALSE;
		} break;

		default:
			return;
		}
	} break;

	default:
		return;
	}
}

void glfw_scroll_cb(GLFWwindow *w, GLdouble x, GLdouble y) {
	(void)w;
	(void)x;
	if (y == 0.0) return;

	GLfloat at_x = GLFW_TO_SCREEN_X(prog.cursor[0]);
	GLfloat at_y = GLFW_TO_SCREEN_Y(prog.cursor[1]);
	GLfloat amt  = 0.0f;

	// Keeps current zoom within bounds with maths.
	if (y > 0.0) {
		if (prog.image.scale >= ZOOM_MAX ||
		    ZOOM_INC >= ZOOM_MAX / prog.image.scale) {
			amt = ZOOM_MAX / prog.image.scale;
		} else amt = ZOOM_INC;
	} else if (prog.image.scale <= ZOOM_MIN ||
	           ZOOM_DEC <= ZOOM_MIN / prog.image.scale) {
		amt = ZOOM_MIN / prog.image.scale;
	} else amt = ZOOM_DEC;

	// Zoom from mouse cursor instead of origin (0, 0).
	prog.image.scale *= amt;
	prog.image.logical_pos[0] = at_x - (at_x - prog.image.logical_pos[0]) * amt;
	prog.image.logical_pos[1] = at_y - (at_y - prog.image.logical_pos[1]) * amt;
	prog.drag_start[0]        = at_x - prog.image.logical_pos[0];
	prog.drag_start[1]        = at_y - prog.image.logical_pos[1];
}