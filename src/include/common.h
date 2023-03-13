#ifndef GLASSES2_COMMON_H_
#define GLASSES2_COMMON_H_
// Commonly used things.

#include "glad.h"
#include "khrplatform.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// All the data needed for one vertex.
typedef struct vertex {
	GLfloat position[2];
	GLfloat uv[2];
} vertex;

// Constants defining how to draw a rectangle.
#define NUM_RECT_VERTICES 4
static const vertex rect_vertices[NUM_RECT_VERTICES] = {
	{
	    {
		0.5f,
		0.5f,
	    },
	    {
		1.0f,
		1.0f,
	    },
	},
	{
	    {
		0.5f,
		-0.5f,
	    },
	    {
		1.0f,
		0.0f,
	    },
	},
	{
	    {
		-0.5f,
		-0.5f,
	    },
	    {
		0.0f,
		0.0f,
	    },
	},
	{
	    {
		-0.5f,
		0.5f,
	    },
	    {
		0.0f,
		1.0f,
	    },
	},
};

#define NUM_RECT_INDICES 6
static const GLuint rect_indices[NUM_RECT_INDICES] = {
	0, 3, 2, 0, 1, 2,
};
#endif // GLASSES2_COMMON_H_
