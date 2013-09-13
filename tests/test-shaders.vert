#version 150
attribute vec2 vertex;
attribute vec3 color;
varying vec3 f_color;

void main( ) {
	gl_Position = vec4( vertex, 0, 1 );
	f_color = color;
}
