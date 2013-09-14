#version 150

in vec3 a_vertex;
in vec3 a_color;
out vec4 f_color;

uniform mat4 model_view;

void main( ) {
	gl_Position = model_view * vec4( a_vertex, 1.0 );
	f_color = vec4(a_color, 1);
}
