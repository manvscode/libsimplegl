#version 100

attribute vec3 a_vertex;
varying vec3 f_vertex;
uniform mat4 u_projection;
uniform vec3 u_position;
uniform int u_width;
uniform int u_height;

void main( )
{
	mat3 scale = mat3( 
		u_width,        0, 0,
		0,       u_height, 0,
		0,              0, 1
	);
	vec3 scaled_vertex = scale * a_vertex;
	vec4 overlay_position = vec4( u_position, 0 ) + vec4( scaled_vertex, 1 );
	gl_Position = u_projection * overlay_position;
	f_vertex = a_vertex;
}
