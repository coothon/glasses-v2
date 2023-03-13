// Stolen from <https://github.com/libretro/common-shaders/blob/master/windowed/shaders/lanczos2-sharp.cg>.
// Translated to GLSL by me.

/*
Original Copyright and License:
   
   Hyllian's lanczos 2-lobe with anti-ringing Shader
   
   Copyright (C) 2011-2014 Hyllian/Jararaca - sergiogdb@gmail.com

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#version 460 core
out vec4 FragColor;

layout (location = 0) uniform sampler2D tex;
uniform int is_lanczos;
uniform vec2 tex_size;

in vec2 uv;

#define LANCZOS2_WINDOW_SINC 0.5
#define LANCZOS2_SINC 1.0
#define LANCZOS_ANTI_RINGING 0.8
#define halfpi  1.5707963267948966192313216916398
#define pi    3.1415926535897932384626433832795
#define wa    (LANCZOS2_WINDOW_SINC*pi)
#define wb    (LANCZOS2_SINC*pi)

// Calculates the distance between two points
float d(vec2 pt1, vec2 pt2) {
	vec2 v = pt2 - pt1;
	return sqrt(dot(v,v));
}

vec3 min4(vec3 a, vec3 b, vec3 c, vec3 d) {
	return min(a, min(b, min(c, d)));
}

vec3 max4(vec3 a, vec3 b, vec3 c, vec3 d) {
	return max(a, max(b, max(c, d)));
}

vec3 lerp(vec3 a, vec3 b, float w) {
	return a + w*(b-a);
}


vec4 lanczos(vec4 x) {
	vec4 res;

	res = (x==vec4(0.0, 0.0, 0.0, 0.0)) ? vec4(wa*wb, wa*wb, wa*wb, wa*wb) : sin(x*wa)*sin(x*wb)/(x*x);

	return res;
}

vec4 lanczos2_sharp(vec2 texture_size, vec2 tex_coord, sampler2D tex_in) {
	vec3 color;
	mat4 weights;

	vec2 dx = vec2(1.0, 0.0);
	vec2 dy = vec2(0.0, 1.0);

	vec2 pc = tex_coord*texture_size;

	vec2 tc = (floor(pc-vec2(0.5,0.5))+vec2(0.5,0.5));

	weights[0] = lanczos(vec4(d(pc, tc    -dx    -dy), d(pc, tc	     -dy), d(pc, tc    +dx    -dy), d(pc, tc+2.0*dx    -dy)));
	weights[1] = lanczos(vec4(d(pc, tc    -dx	 ), d(pc, tc		  ), d(pc, tc    +dx	 ), d(pc, tc+2.0*dx	 )));
	weights[2] = lanczos(vec4(d(pc, tc    -dx    +dy), d(pc, tc	     +dy), d(pc, tc    +dx    +dy), d(pc, tc+2.0*dx    +dy)));
	weights[3] = lanczos(vec4(d(pc, tc    -dx+2.0*dy), d(pc, tc	 +2.0*dy), d(pc, tc    +dx+2.0*dy), d(pc, tc+2.0*dx+2.0*dy)));

	dx = dx/texture_size;
	dy = dy/texture_size;
	tc = tc/texture_size;

	// reading the texels

	vec3 c00 = texture2D(tex_in, tc    -dx    -dy).xyz;
	vec3 c10 = texture2D(tex_in, tc           -dy).xyz;
	vec3 c20 = texture2D(tex_in, tc    +dx    -dy).xyz;
	vec3 c30 = texture2D(tex_in, tc+2.0*dx    -dy).xyz;
	vec3 c01 = texture2D(tex_in, tc    -dx       ).xyz;
	vec3 c11 = texture2D(tex_in, tc              ).xyz;
	vec3 c21 = texture2D(tex_in, tc    +dx       ).xyz;
	vec3 c31 = texture2D(tex_in, tc+2.0*dx       ).xyz;
	vec3 c02 = texture2D(tex_in, tc    -dx    +dy).xyz;
	vec3 c12 = texture2D(tex_in, tc           +dy).xyz;
	vec3 c22 = texture2D(tex_in, tc    +dx    +dy).xyz;
	vec3 c32 = texture2D(tex_in, tc+2.0*dx    +dy).xyz;
	vec3 c03 = texture2D(tex_in, tc    -dx+2.0*dy).xyz;
	vec3 c13 = texture2D(tex_in, tc       +2.0*dy).xyz;
	vec3 c23 = texture2D(tex_in, tc    +dx+2.0*dy).xyz;
	vec3 c33 = texture2D(tex_in, tc+2.0*dx+2.0*dy).xyz;

	//  Get min/max samples
	vec3 min_sample = min4(c11, c21, c12, c22);
	vec3 max_sample = max4(c11, c21, c12, c22);

	color = mat4x3(c00, c10, c20, c30) * weights[0];
	color+= mat4x3(c01, c11, c21, c31) * weights[1];
	color+= mat4x3(c02, c12, c22, c32) * weights[2];
	color+= mat4x3(c03, c13, c23, c33) * weights[3];
	color = color/(dot(weights * vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0)));

	// Anti-ringing
	vec3 aux = color;
	color = clamp(color, min_sample, max_sample);

	color = lerp(aux, color, LANCZOS_ANTI_RINGING);

	// final sum and weight normalization
	return vec4(color, 1.0);
}

void main(void) {
	FragColor = is_lanczos != 0 ? lanczos2_sharp(tex_size, uv, tex) : texture2D(tex, uv);
}
