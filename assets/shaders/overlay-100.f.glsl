#version 100

varying highp vec3 f_vertex;
uniform highp vec3 u_color;
uniform sampler2D u_texture;
uniform int u_width;
uniform int u_height;
uniform bool u_use_texture;

void main( )
{
	highp vec2 texture_coord = f_vertex.xy;
	highp vec4 texel = texture2D( u_texture, texture_coord );

	if( u_use_texture )
	{
		gl_FragColor = mix( vec4(u_color, 1), texel, texel.a );
		gl_FragColor = texel;
	}
	else
	{
		gl_FragColor = vec4(u_color, 1);
	}
}
