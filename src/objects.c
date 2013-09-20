/* Copyright (C) 2013 by Joseph A. Marrero, http://www.manvscode.com/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
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
	// top
	-1.0,  1.0,  1.0,
	1.0,  1.0,  1.0,
	1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0,
	// back
	1.0, -1.0, -1.0,
	-1.0, -1.0, -1.0,
	-1.0,  1.0, -1.0,
	1.0,  1.0, -1.0,
	// bottom
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0, -1.0,  1.0,
	-1.0, -1.0,  1.0,
	// left
	-1.0, -1.0, -1.0,
	-1.0, -1.0,  1.0,
	-1.0,  1.0,  1.0,
	-1.0,  1.0, -1.0,
	// right
	1.0, -1.0,  1.0,
	1.0, -1.0, -1.0,
	1.0,  1.0, -1.0,
	1.0,  1.0,  1.0,
};

static GLushort cube_indices[] = {
	// front
	0,  1,  2,
	2,  3,  0,
	// top
	4,  5,  6,
	6,  7,  4,
	// back
	8,  9, 10,
	10, 11,  8,
	// bottom
	12, 13, 14,
	14, 15, 12,
	// left
	16, 17, 18,
	18, 19, 16,
	// right
	20, 21, 22,
	22, 23, 20,
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
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	// back colors
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	//
	1.0, 1.0, 0.0,
	1.0, 1.0, 0.0,
	1.0, 1.0, 0.0,
	1.0, 1.0, 0.0,
	//
	0.0, 1.0, 1.0,
	0.0, 1.0, 1.0,
	0.0, 1.0, 1.0,
	0.0, 1.0, 1.0,
	//
	1.0, 0.0, 1.0,
	1.0, 0.0, 1.0,
	1.0, 0.0, 1.0,
	1.0, 0.0, 1.0,
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

