#version 150

in vec3 a_vertex;
out vec3 f_tex_coord;

uniform vec3 u_eye_position;
uniform mat4 u_projection;
uniform mat4 u_orientation;

void main( ) {
	gl_Position =  u_projection * u_orientation * vec4( a_vertex, 1.0 );
	f_tex_coord = a_vertex;
	//f_tex_coord = u_eye_position - a_vertex;
}
