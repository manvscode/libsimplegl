#version 120

attribute vec3 a_vertex;

void main()
{
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = a_vertex;
}
