#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <libcollections/.h>
#include <assert.h>
#include "simplegl.h"


#define SQRT_OF_2       (1.414213562373095)

static const GLfloat tetrahedron_vertices[] = {
	 1.0,  0.0, -1.0/SQRT_OF_2,
	-1.0,  0.0, -1.0/SQRT_OF_2,
	 0.0,  1.0,  1.0/SQRT_OF_2,
	 0.0, -1.0,  1.0/SQRT_OF_2,
};

static GLushort tetrahedron_indices[] = {
	0, 1, 2,
	0, 1, 3,
	0, 2, 1,

	0, 2, 3,
	0, 3, 1,
	0, 3, 2,

	1, 0, 2,
	1, 0, 3,
	1, 2, 3,

	1, 2, 0,
	1, 3, 2,
	1, 3, 0,
/*
	2, 0, 1,
	2, 0, 3,
	2, 1, 0,

	2, 1, 3,
	2, 3, 0,
	2, 3, 1,

	3, 0, 1,
	3, 0, 2,
	3, 1, 0,

	3, 1, 2,
	3, 2, 0,
	3, 2, 1
*/
};
static GLfloat tetrahedron_tex_coords[] = {
	0.0, 0.0,
	3.0, 0.0,
	1.5, 2.59807621,

	0.0, 0.0,
	3.0, 0.0,
	1.5, 2.59807621,

	0.0, 0.0,
	3.0, 0.0,
	1.5, 2.59807621,

	0.0, 0.0,
	3.0, 0.0,
	1.5, 2.59807621
};

static const GLfloat tetrahedron_colors[] = {
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 1.0, 1.0,
};



static GLfloat cube_vertices[] = {
	// front
	-1.0, -1.0,  1.0,
	1.0, -1.0,  1.0,
	1.0,  1.0,  1.0,
	-1.0,  1.0,  1.0,
	// back
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0,
};

static GLushort cube_indices[] = {
	// front
	0, 1, 2,
	2, 3, 0,
	// top
	3, 2, 6,
	6, 7, 3,
	// back
	7, 6, 5,
	5, 4, 7,
	// bottom
	4, 5, 1,
	1, 0, 4,
	// left
	4, 0, 3,
	3, 7, 4,
	// right
	1, 5, 6,
	6, 2, 1,
};

static GLfloat cube_tex_coords[] = {
	// front
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,
	//
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,
	//
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,
	//
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,
	//
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,
	//
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0
};

static GLfloat cube_colors[] = {
	// front colors
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 1.0, 1.0,
	// back colors
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 1.0, 1.0,
};

void polyhedra_create( polyhedra_t* polyhedra )
{
	if( polyhedra )
	{
		polyhedra->vertices         = NULL;
		polyhedra->vertices_count   = 0;
		polyhedra->indices          = NULL;
		polyhedra->indices_count    = 0;
		polyhedra->tex_coords       = NULL;
		polyhedra->tex_coords_count = 0;
		polyhedra->colors           = NULL;
		polyhedra->colors_count     = 0;
	}
}

void polyhedra_destroy( polyhedra_t* polyhedra )
{
	if( polyhedra )
	{
		free( polyhedra->vertices );
		free( polyhedra->indices );
		free( polyhedra->tex_coords );
		free( polyhedra->colors );
	}
}

GLboolean tetrahedron( polyhedra_t* polyhedra, GLfloat scale )
{
	if( polyhedra )
	{
		polyhedra->vertices_count = sizeof(tetrahedron_vertices) / sizeof(tetrahedron_vertices[0]);
		polyhedra->vertices = (GLfloat*) malloc( sizeof(GLfloat) * polyhedra->vertices_count );
		if( !polyhedra->vertices ) goto failure;

		for( int i = 0; i < polyhedra->vertices_count; i++ )
		{
			polyhedra->vertices[ i ] = tetrahedron_vertices[ i ] * scale;
		}

		polyhedra->indices_count = sizeof(tetrahedron_indices) / sizeof(tetrahedron_indices[0]);
		polyhedra->indices = (GLushort*) malloc( sizeof(GLushort) * polyhedra->indices_count );
		if( !polyhedra->indices ) goto failure;
		memcpy( polyhedra->indices, tetrahedron_indices, polyhedra->indices_count * sizeof(GLushort) );

		polyhedra->tex_coords_count = sizeof(tetrahedron_tex_coords) / sizeof(tetrahedron_tex_coords[0]);
		polyhedra->tex_coords = (GLfloat*) malloc( sizeof(GLfloat) * polyhedra->tex_coords_count );
		if( !polyhedra->tex_coords ) goto failure;
		memcpy( polyhedra->tex_coords, tetrahedron_tex_coords, polyhedra->tex_coords_count * sizeof(GLfloat) );

		polyhedra->colors_count = sizeof(tetrahedron_colors) / sizeof(tetrahedron_colors[0]);
		polyhedra->colors = (GLfloat*) malloc( sizeof(GLfloat) * polyhedra->colors_count );
		if( !polyhedra->colors ) goto failure;
		memcpy( polyhedra->colors, tetrahedron_colors, polyhedra->colors_count * sizeof(GLfloat) );

	}

	return GL_TRUE;
failure:
	if( polyhedra->vertices )   free( polyhedra->vertices );
	if( polyhedra->indices )    free( polyhedra->indices );
	if( polyhedra->tex_coords ) free( polyhedra->tex_coords );
	if( polyhedra->colors )     free( polyhedra->colors );

	return GL_FALSE;
}

GLboolean cube( polyhedra_t* polyhedra, GLfloat scale )
{
	if( polyhedra )
	{
		polyhedra->vertices_count = sizeof(cube_vertices) / sizeof(cube_vertices[0]);
		polyhedra->vertices = (GLfloat*) malloc( sizeof(GLfloat) * polyhedra->vertices_count );
		assert( polyhedra->vertices );
		if( !polyhedra->vertices ) goto failure;

		for( int i = 0; i < polyhedra->vertices_count; i++ )
		{
			polyhedra->vertices[ i ] = cube_vertices[ i ] * scale;
		}

		polyhedra->indices_count = sizeof(cube_indices) / sizeof(cube_indices[0]);
		polyhedra->indices = (GLushort*) malloc( sizeof(GLushort) * polyhedra->indices_count );
		assert( polyhedra->indices );
		if( !polyhedra->indices ) goto failure;
		memcpy( polyhedra->indices, cube_indices, polyhedra->indices_count * sizeof(GLushort) );

		polyhedra->tex_coords_count = sizeof(cube_tex_coords) / sizeof(cube_tex_coords[0]);
		polyhedra->tex_coords = (GLfloat*) malloc( sizeof(GLfloat) * polyhedra->tex_coords_count );
		assert( polyhedra->tex_coords );
		if( !polyhedra->tex_coords ) goto failure;
		memcpy( polyhedra->tex_coords, cube_tex_coords, polyhedra->tex_coords_count * sizeof(GLfloat) );

		polyhedra->colors_count = sizeof(cube_colors) / sizeof(cube_colors[0]);
		polyhedra->colors = (GLfloat*) malloc( sizeof(GLfloat) * polyhedra->colors_count );
		assert( polyhedra->colors );
		if( !polyhedra->colors ) goto failure;
		memcpy( polyhedra->colors, cube_colors, polyhedra->colors_count * sizeof(GLfloat) );
	}

	return GL_TRUE;
failure:
	if( polyhedra->vertices )   free( polyhedra->vertices );
	if( polyhedra->indices )    free( polyhedra->indices );
	if( polyhedra->tex_coords ) free( polyhedra->tex_coords );
	if( polyhedra->colors )     free( polyhedra->colors );

	return GL_FALSE;
}

