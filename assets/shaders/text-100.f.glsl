#version 100
varying highp vec3 f_vertex;
uniform highp vec3 u_color;
uniform sampler2D u_texture;
uniform highp float u_width;
uniform highp float u_height;
uniform highp float u_glyph_width;
uniform highp float u_glyph_height;
uniform int u_character;

void main( )
{
	highp vec2 offset = vec2( 0, 1.0 - (float(u_character + 1) * u_glyph_height) / u_height );

	highp mat2 scale = mat2( 
		u_glyph_width / u_width,                         0,
		0,                       u_glyph_height / u_height
	);


	#if 1
	highp vec2 texture_coord = offset + scale * vec2( f_vertex.x, f_vertex.y );
	#else
	highp vec2 texture_coord = f_vertex.xy;
	#endif
	highp vec4 texel = texture2D( u_texture, texture_coord );
	gl_FragColor = vec4(u_color, 1) * vec4( texel.r );
}
