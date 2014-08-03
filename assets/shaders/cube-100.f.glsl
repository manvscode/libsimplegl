#version 100

varying highp vec2 f_tex_coord;
varying highp vec4 f_color;
uniform sampler2D u_texture;

void main( ) {
	gl_FragColor = f_color + texture2D( u_texture, f_tex_coord );
}
