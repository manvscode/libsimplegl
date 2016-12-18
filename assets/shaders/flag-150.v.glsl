#version 400

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

out vec3 f_normal;
out vec2 f_tex_coord;
out vec4 f_color;

uniform mat4 u_model_view;
uniform int u_time;

const vec4 COLOR = vec4( 1, 1, 1, 1 );

void main( ) {
	float A = 5 * sin(a_vertex.x * 0.1 + u_time * 0.01);
	float B = 5 * cos(a_vertex.y * 0.1 + u_time * 0.01);

	gl_Position = u_model_view * vec4( a_vertex.x, a_vertex.y, A + a_vertex.z, 1.0 );
	f_normal    = a_normal;
	f_tex_coord = vec2( (a_vertex.x + 154 / 2.0) / 153.0, (a_vertex.y + 100 / 2.0) / 99.0 );
	f_color = COLOR;
}
