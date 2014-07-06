#version 150
in vec3 f_vertex;
out vec4 color;
uniform vec3 u_color;
uniform sampler2D u_texture;
uniform float u_width;
uniform float u_height;
uniform float u_glyph_width;
uniform float u_glyph_height;
uniform int u_character;

void main( )
{
	vec2 offset = vec2( 0, 1 - ((u_character + 1) * u_glyph_height / u_height) );

	mat2 scale = mat2( 
		u_glyph_width / u_width,                         0,
		0,                       u_glyph_height / u_height
	);


	#if 1
	vec2 texture_coord = offset + scale * vec2( f_vertex.x, f_vertex.y );
	#else
	vec2 texture_coord = f_vertex.xy;
	#endif
	vec4 texel = texture( u_texture, texture_coord );
	color = vec4(u_color, 1) * vec4( texel.r );
}
