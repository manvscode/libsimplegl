#version 150

in vec3 a_vertex;
in vec2 a_tex_coord;
in vec3 a_color;
out vec2 f_tex_coord;
out vec4 f_color;

uniform mat4 model_view;

void main( ) {
	gl_Position = model_view * vec4( a_vertex, 1.0 );
	f_tex_coord = a_tex_coord;	
	f_color = vec4(a_color, 1);
}
