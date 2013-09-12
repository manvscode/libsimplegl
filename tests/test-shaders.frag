#version 300 
in vec3 f_color;

void main( ) {
	gl_FragColor = vec4( f_color.x, f_color.y, f_color.z, 1.0 );
}
