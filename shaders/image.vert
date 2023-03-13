#version 460 core
layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 v_uv;

uniform vec2 viewport_size;
uniform vec2 tex_size;
uniform vec2 logical_pos;
uniform float scale;
uniform float perfect_scale; // Scales with window size: bigger window = can zoom more.

out vec2 uv;

vec2 screen_project(vec2 pos) {
	return ((pos / viewport_size) * 2.0) - 1.0;
}

void main(void) {
	uv = v_uv;
	vec2 tex_ratio = v_pos * tex_size;
	gl_Position = vec4(screen_project((tex_ratio * scale * perfect_scale) + (viewport_size * 0.5) + logical_pos), 0.0, 1.0);
}
