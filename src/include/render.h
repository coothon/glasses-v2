#ifndef GLASSES2_RENDER_H_
#define GLASSES2_RENDER_H_

#define _DEFAULT_SOURCE 1
#include "glad.h"
#include "khrplatform.h"
#define GLFW_INCLUDE_NONE
#include "stb_image.h"
#include <GLFW/glfw3.h>

#include "common.h"
#include "conf.h"
#include "console.h"
#include "list.h"

#define GLFW_TO_SCREEN_X(pos) \
(GLfloat)((pos)) - ((GLfloat)(prog.window_size[0]) / 2.0f)
#define GLFW_TO_SCREEN_Y(pos) \
-((GLfloat)((pos)) - ((GLfloat)(prog.window_size[1]) / 2.0f))

// Multiplies by 1.1f.
#define ZOOM_INC 1.1f
#define ZOOM_DEC (1.0f / ZOOM_INC)
#define ZOOM_MAX 25.0f
#define ZOOM_MIN 0.5f
#define MOVE_AMOUNT 150.0f

typedef struct renderer {
	GLuint  VAO;
	GLuint  VBO;
	GLuint  EBO;
	GLuint  vert_shader;
	GLuint  frag_shader;
	GLuint  shader_program;
	vertex *vertices;
	GLuint *indices;
	GLint   num_vertices;
	GLint   num_indices;
	GLuint  texture;
	GLuint  uniform_is_lanczos;
	GLuint  uniform_texsize;
	GLuint  uniform_scale;
	GLuint  uniform_perfect_scale;
	GLuint  uniform_logical_pos;
	GLuint  uniform_viewport_size;
	GLfloat logical_pos[2];
	GLfloat scale;
} renderer;

typedef struct glasses {
	int         many_files_total_count;
	int         many_files_current_index;
	list_node  *many_files;
	GLchar     *many_files_dir;
	GLdouble    total_time;
	GLdouble    cursor[2];
	GLboolean   drag_mode;
	GLfloat     drag_start[2];
	stbi_uc    *imgdata;
	GLint       imgwidth, imgheight, imgchannels;
	GLfloat     imgwidth_f, imgheight_f;
	renderer    image;
	GLint       window_size[2];
	const char *current_image;
	GLFWwindow *win;
} glasses;
extern glasses prog;

GLint      renderer_init(glasses *g);
GLint      many_files_prev(glasses *g);
GLint      many_files_next(glasses *g);
list_node *many_files_load(list_node *file_node);
GLint      renderer_send_image(glasses *g);

void glfw_error_cb(GLint e, const GLchar *desc);
void glfw_key_cb(GLFWwindow *w, GLint key, GLint scancode, GLint action,
                 GLint mods);
void glfw_framebuffer_size_cb(GLFWwindow *w, GLint fbwidth, GLint fbheight);
void glfw_cursorpos_cb(GLFWwindow *w, GLdouble x, GLdouble y);
void glfw_click_cb(GLFWwindow *w, GLint button, GLint action, GLint mods);
void glfw_scroll_cb(GLFWwindow *w, GLdouble x, GLdouble y);

#endif // GLASSES2_RENDER_H_
