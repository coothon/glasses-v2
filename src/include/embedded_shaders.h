const char *__shaders_image_vert =
    "#version 460 core\n"
    "layout (location = 0) in vec2 v_pos;\n"
    "layout (location = 1) in vec2 v_uv;\n"
    "\n"
    "uniform vec2 viewport_size;\n"
    "uniform vec2 tex_size;\n"
    "uniform vec2 logical_pos;\n"
    "uniform float scale;\n"
    "uniform float perfect_scale; // Scales with window size: bigger window = can zoom more.\n"
    "\n"
    "out vec2 uv;\n"
    "\n"
    "vec2 screen_project(vec2 pos) {\n"
    "	return ((pos / viewport_size) * 2.0) - 1.0;\n"
    "}\n"
    "\n"
    "void main(void) {\n"
    "	uv = v_uv;\n"
    "	vec2 tex_ratio = v_pos * tex_size;\n"
    "	gl_Position = vec4(screen_project((tex_ratio * scale * perfect_scale) + (viewport_size * 0.5) + logical_pos), 0.0, 1.0);\n"
    "}\n";

const char *__shaders_image_frag =
    "// Stolen from <https://github.com/libretro/common-shaders/blob/master/windowed/shaders/lanczos2-sharp.cg>.\n"
    "// Translated to GLSL by me.\n"
    "\n"
    "/*\n"
    "Original Copyright and License:\n"
    "   \n"
    "   Hyllian's lanczos 2-lobe with anti-ringing Shader\n"
    "   \n"
    "   Copyright (C) 2011-2014 Hyllian/Jararaca - sergiogdb@gmail.com\n"
    "\n"
    "   This program is free software; you can redistribute it and/or\n"
    "   modify it under the terms of the GNU General Public License\n"
    "   as published by the Free Software Foundation; either version 2\n"
    "   of the License, or (at your option) any later version.\n"
    "\n"
    "   This program is distributed in the hope that it will be useful,\n"
    "   but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "   GNU General Public License for more details.\n"
    "\n"
    "   You should have received a copy of the GNU General Public License\n"
    "   along with this program; if not, write to the Free Software\n"
    "   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n"
    "*/\n"
    "#version 460 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "layout (location = 0) uniform sampler2D tex;\n"
    "uniform int is_lanczos;\n"
    "uniform vec2 tex_size;\n"
    "\n"
    "in vec2 uv;\n"
    "\n"
    "#define LANCZOS2_WINDOW_SINC 0.5\n"
    "#define LANCZOS2_SINC 1.0\n"
    "#define LANCZOS_ANTI_RINGING 0.8\n"
    "#define halfpi  1.5707963267948966192313216916398\n"
    "#define pi    3.1415926535897932384626433832795\n"
    "#define wa    (LANCZOS2_WINDOW_SINC*pi)\n"
    "#define wb    (LANCZOS2_SINC*pi)\n"
    "\n"
    "// Calculates the distance between two points\n"
    "float d(vec2 pt1, vec2 pt2) {\n"
    "	vec2 v = pt2 - pt1;\n"
    "	return sqrt(dot(v,v));\n"
    "}\n"
    "\n"
    "vec3 min4(vec3 a, vec3 b, vec3 c, vec3 d) {\n"
    "	return min(a, min(b, min(c, d)));\n"
    "}\n"
    "\n"
    "vec3 max4(vec3 a, vec3 b, vec3 c, vec3 d) {\n"
    "	return max(a, max(b, max(c, d)));\n"
    "}\n"
    "\n"
    "vec3 lerp(vec3 a, vec3 b, float w) {\n"
    "	return a + w*(b-a);\n"
    "}\n"
    "\n"
    "\n"
    "vec4 lanczos(vec4 x) {\n"
    "	vec4 res;\n"
    "\n"
    "	res = (x==vec4(0.0, 0.0, 0.0, 0.0)) ? vec4(wa*wb, wa*wb, wa*wb, wa*wb) : sin(x*wa)*sin(x*wb)/(x*x);\n"
    "\n"
    "	return res;\n"
    "}\n"
    "\n"
    "vec4 lanczos2_sharp(vec2 texture_size, vec2 tex_coord, sampler2D tex_in) {\n"
    "	vec3 color;\n"
    "	mat4 weights;\n"
    "\n"
    "	vec2 dx = vec2(1.0, 0.0);\n"
    "	vec2 dy = vec2(0.0, 1.0);\n"
    "\n"
    "	vec2 pc = tex_coord*texture_size;\n"
    "\n"
    "	vec2 tc = (floor(pc-vec2(0.5,0.5))+vec2(0.5,0.5));\n"
    "\n"
    "	weights[0] = lanczos(vec4(d(pc, tc    -dx    -dy), d(pc, tc	     -dy), d(pc, tc    +dx    -dy), d(pc, tc+2.0*dx    -dy)));\n"
    "	weights[1] = lanczos(vec4(d(pc, tc    -dx	 ), d(pc, tc		  ), d(pc, tc    +dx	 ), d(pc, tc+2.0*dx	 )));\n"
    "	weights[2] = lanczos(vec4(d(pc, tc    -dx    +dy), d(pc, tc	     +dy), d(pc, tc    +dx    +dy), d(pc, tc+2.0*dx    +dy)));\n"
    "	weights[3] = lanczos(vec4(d(pc, tc    -dx+2.0*dy), d(pc, tc	 +2.0*dy), d(pc, tc    +dx+2.0*dy), d(pc, tc+2.0*dx+2.0*dy)));\n"
    "\n"
    "	dx = dx/texture_size;\n"
    "	dy = dy/texture_size;\n"
    "	tc = tc/texture_size;\n"
    "\n"
    "	// reading the texels\n"
    "\n"
    "	vec3 c00 = texture2D(tex_in, tc    -dx    -dy).xyz;\n"
    "	vec3 c10 = texture2D(tex_in, tc           -dy).xyz;\n"
    "	vec3 c20 = texture2D(tex_in, tc    +dx    -dy).xyz;\n"
    "	vec3 c30 = texture2D(tex_in, tc+2.0*dx    -dy).xyz;\n"
    "	vec3 c01 = texture2D(tex_in, tc    -dx       ).xyz;\n"
    "	vec3 c11 = texture2D(tex_in, tc              ).xyz;\n"
    "	vec3 c21 = texture2D(tex_in, tc    +dx       ).xyz;\n"
    "	vec3 c31 = texture2D(tex_in, tc+2.0*dx       ).xyz;\n"
    "	vec3 c02 = texture2D(tex_in, tc    -dx    +dy).xyz;\n"
    "	vec3 c12 = texture2D(tex_in, tc           +dy).xyz;\n"
    "	vec3 c22 = texture2D(tex_in, tc    +dx    +dy).xyz;\n"
    "	vec3 c32 = texture2D(tex_in, tc+2.0*dx    +dy).xyz;\n"
    "	vec3 c03 = texture2D(tex_in, tc    -dx+2.0*dy).xyz;\n"
    "	vec3 c13 = texture2D(tex_in, tc       +2.0*dy).xyz;\n"
    "	vec3 c23 = texture2D(tex_in, tc    +dx+2.0*dy).xyz;\n"
    "	vec3 c33 = texture2D(tex_in, tc+2.0*dx+2.0*dy).xyz;\n"
    "\n"
    "	//  Get min/max samples\n"
    "	vec3 min_sample = min4(c11, c21, c12, c22);\n"
    "	vec3 max_sample = max4(c11, c21, c12, c22);\n"
    "\n"
    "	color = mat4x3(c00, c10, c20, c30) * weights[0];\n"
    "	color+= mat4x3(c01, c11, c21, c31) * weights[1];\n"
    "	color+= mat4x3(c02, c12, c22, c32) * weights[2];\n"
    "	color+= mat4x3(c03, c13, c23, c33) * weights[3];\n"
    "	color = color/(dot(weights * vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0)));\n"
    "\n"
    "	// Anti-ringing\n"
    "	vec3 aux = color;\n"
    "	color = clamp(color, min_sample, max_sample);\n"
    "\n"
    "	color = lerp(aux, color, LANCZOS_ANTI_RINGING);\n"
    "\n"
    "	// final sum and weight normalization\n"
    "	return vec4(color, 1.0);\n"
    "}\n"
    "\n"
    "void main(void) {\n"
    "	FragColor = is_lanczos != 0 ? lanczos2_sharp(tex_size, uv, tex) : texture2D(tex, uv);\n"
    "}\n";