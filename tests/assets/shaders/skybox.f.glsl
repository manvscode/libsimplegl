#version 150

in vec3 f_tex_coord;
uniform samplerCube u_cubemap;

out vec4 color;

void main( ) {
	color = texture( u_cubemap, f_tex_coord );
}
