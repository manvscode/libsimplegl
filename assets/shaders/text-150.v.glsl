#version 150
in vec3 a_vertex;
out vec3 f_vertex;
uniform mat4 u_projection;
uniform vec3 u_position;
uniform uint u_width;
uniform uint u_height;
uniform uint u_glyph_width;
uniform uint u_glyph_height;
uniform float u_size;

void main( )
{
	#if 1
	mat3 scale = mat3( 
		u_glyph_width,              0, 0,
		            0, u_glyph_height, 0,
		            0,              0, 1
	);
	#else
	mat3 scale = mat3( 
		u_width,        0, 0,
		0,       u_height, 0,
		0,              0, 1
	);
	#endif
	vec3 scaled_vertex = u_size * scale * a_vertex;
	vec4 character_position = vec4( u_position, 0 ) + vec4( scaled_vertex, 1 );
	gl_Position = u_projection * character_position;
	f_vertex = a_vertex;
}
