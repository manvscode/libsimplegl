#version 130
attribute vec2 vertex;
//attribute vec3 color;

//varying vec3 f_color;

void main( void )
{
	gl_Position = vec4( vertex, 0.0, 1.0 );
	//f_color = color;
}
