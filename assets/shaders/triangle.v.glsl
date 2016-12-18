#version 400

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_color;

out vec4 f_color;

uniform mat4 u_model_view;

void main( ) {
	gl_Position = u_model_view * vec4( a_vertex, 1.0 );
	f_color = vec4(a_color, 1);
}
