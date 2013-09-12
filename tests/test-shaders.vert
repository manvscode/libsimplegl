#version 330 core 
in vec2 vertex;
in vec3 color;
out vec3 f_color;

void main( ) {
	gl_Position = vec4( vertex, 0.0, 1.0 );
	f_color = color;
}
