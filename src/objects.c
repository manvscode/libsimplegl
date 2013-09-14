#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "simplegl.h"

/*
v  1  0  0
v  0  -1  0
v  -1  0  0
v  0  1  0
v  0  0  1
v  0  0  -1

f  2  1  5
f  3  2  5
f  4  3  5
f  1  4  5
f  1  2  6
f  2  3  6
f  3  4  6
f  4  1  6

*/

static const GLfloat tetrahedron_list[] = {
	0,-1,0,   1,0,0,   0,0,1,
	-1,0,0,   0,-1,0,   0,0,1,
	0,1,0,   -1,0,0,   0,0,1,
	1,0,0,   0,1,0,   0,0,1,
	1,0,0,   0,-1,0,   0,0,-1,
	0,-1,0,   -1,0,0,   0,0,-1,
	-1,0,0,   0,1,0,   0,0,-1,
	0,1,0,   1,0,0,   0,0,-1
};



static const GLfloat cube_vertices_strip[] = {
	// Front face
	-1,-1,1,  1,-1,1,  -1,1,1,  -1,1,1,  1,-1,1,  1,1,1,
	//  Back  face
	1,-1,-1,  -1,-1,-1,  1,1,-1,  1,1,-1,  -1,-1,-1,  -1,1,-1,
	//  Right  face
	1,-1,1,  1,-1,-1,  1,1,1,   1,1,1,  1,-1,-1,   1,1,-1,
	//  Left  face
	-1,-1,-1,  -1,-1,1,  -1,1,-1,  -1,1,-1,  -1,-1,1,  -1,1,1,
	//  Bottom  face
	-1,-1,-1,  1,-1,-1,  -1,-1,1,  -1,-1,1,  1,-1,-1,   1,-1,1,
	//  Top  face
	-1,1,1,  1,1,1,  -1,1,-1,  -1,1,-1,  1,1,1,   1,1,-1
};

static const GLfloat cube_tex_coords_strip[] = {
        // Front face
        0,0, 1,0, 0,1, 1,1,
        // Right face
        0,0, 1,0, 0,1, 1,1,
        // Back face
        0,0, 1,0, 0,1, 1,1,
        // Left face
        0,0, 1,0, 0,1, 1,1,
        // Bottom face
        0,0, 1,0, 0,1, 1,1,
        // Top face
        0,0, 1,0, 0,1, 1,1
    };

GLfloat* tetrahedron( GLfloat scale, GLsizei* size )
{
	GLsizei array_size = sizeof(tetrahedron_list) / sizeof(tetrahedron_list[0]);
	GLfloat* result = (GLfloat*) malloc( sizeof(GLfloat) * array_size );

	if( result )
	{
		for( GLsizei i = 0; i < array_size; i++ )
		{
			result[ i ] = tetrahedron_list[ i ] * scale;
		}

		*size = array_size;
		printf( "Vertices in Tetrahedon: %d\n", *size / 3 );
	}

	return result;
}

GLfloat* cube( GLfloat scale, GLsizei* size )
{
	GLsizei strip_size = sizeof(cube_vertices_strip) / sizeof(cube_vertices_strip[0]);
	GLfloat* result = (GLfloat*) malloc( sizeof(GLfloat) * strip_size );

	if( result )
	{
		for( GLsizei i = 0; i < strip_size; i++ )
		{
			result[ i ] = cube_vertices_strip[ i ] * scale;
		}

		*size = strip_size;

		printf( "Vertices in Cube: %d\n", *size / 3 );
	}

	return result;
}

GLfloat* cube_tex_coords( void )
{
	GLsizei strip_size = sizeof(cube_tex_coords_strip) / sizeof(cube_tex_coords_strip[0]);
	GLfloat* result = (GLfloat*) malloc( sizeof(GLfloat) * strip_size );

	if( result )
	{
		memcpy( result, cube_tex_coords_strip, strip_size );
	}

	return result;
}
