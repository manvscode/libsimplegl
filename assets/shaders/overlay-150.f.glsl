#version 150

in vec3 f_vertex;
out vec4 color;
uniform vec3 u_color;
uniform sampler2D u_texture;
uniform int u_width;
uniform int u_height;
uniform bool u_use_texture;

void main( )
{
	vec2 texture_coord = f_vertex.xy;
	vec4 texel = texture( u_texture, texture_coord );

	if( u_use_texture )
	{
		color = mix( vec4(u_color, 1), texel, texel.a );
		color = texel;
	}
	else
	{
		color = vec4(u_color, 1);
	}
}
