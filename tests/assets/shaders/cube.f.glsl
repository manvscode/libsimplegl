#version 400

in vec2 f_tex_coord;
in vec4 f_color;
uniform sampler2D u_texture;

out vec4 color;

void main( ) {
	color = f_color + texture( u_texture, f_tex_coord );

	//if( color.r >= 0.9f && color.g >= 0.9f && color.b >= 0.9f ) 
		//discard;
}
