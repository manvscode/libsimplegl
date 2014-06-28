#version 150

in vec3 f_vertex;
out vec4 color;
uniform vec3 u_color;
uniform sampler2D u_texture;
uniform uint u_width;
uniform uint u_height;
uniform uint u_glyph_width;
uniform uint u_glyph_height;
uniform uint u_character;

void main( )
{
	vec2 offset = vec2( 0, 1 - ((u_character + 1u) * u_glyph_height / 1024.0) );

	mat2 scale = mat2( 
		u_glyph_width / float(u_width),                         0,
		0,                       u_glyph_height / float(u_height)
	);


	#if 1
	vec2 texture_coord = offset + scale * vec2( f_vertex.x, f_vertex.y );
	#else
	vec2 texture_coord = f_vertex.xy;
	#endif
	vec4 texel = texture( u_texture, texture_coord );
	color = vec4(u_color, 1) * vec4( texel.r );
}
