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
#include <assert.h>
#include "simplegl.h"
#include <lib3dmath/vec3.h>
#include <lib3dmath/quat.h>
#include <lib3dmath/easing.h>

#define CAMERA_EPSILON                 (0.1f)



camera_t* camera_create( int screen_width, int screen_height, GLfloat near, GLfloat far, GLfloat fov, const pt3_t* position )
{
	camera_t* c = malloc( sizeof(camera_t) );

	if( c )
	{
		const GLfloat aspect = ((GLfloat)screen_width) / screen_height;

		c->position           = position ? *position : VEC3_ZERO;
        c->xangle             = 0.0f;
        c->yangle             = 0.0f;
		c->projection_matrix  = perspective( fov * RADIANS_PER_DEGREE, aspect, near, far );
        c->model_matrix       = MAT4_IDENTITY;
        c->orientation_matrix = MAT4_IDENTITY;
		vec3_negate( &c->position );

		#ifdef SIMPLEGL_DEBUG
		printf( "   Camera position: %s\n", vec3_to_string(&c->position) );
		#endif
	}

	assert( c != NULL );
	return c;
}

void camera_destroy( camera_t* camera )
{
	free( camera );
}

const mat4_t* camera_projection_matrix( const camera_t* camera )
{
	assert( camera );
	return &camera->projection_matrix;
}

const mat4_t* camera_model_matrix( const camera_t* camera )
{
	assert( camera );
    return &camera->model_matrix;
}

const mat4_t* camera_orientation_matrix( const camera_t* camera )
{
	assert( camera );
	return &camera->orientation_matrix;
}

mat3_t camera_normal_matrix( const camera_t* camera )
{
    assert( camera );
	return MAT3(
		camera->model_matrix.m[0], camera->model_matrix.m[1], camera->model_matrix.m[ 2],
		camera->model_matrix.m[4], camera->model_matrix.m[5], camera->model_matrix.m[ 6],
		camera->model_matrix.m[8], camera->model_matrix.m[9], camera->model_matrix.m[10]
	);
}

mat4_t camera_view_matrix( const camera_t* camera )
{
	assert( camera );
	#ifdef SIMPLEGL_DEBUG
	//printf( "     Camera model: %s\n", mat4_to_string(&model) );
	//printf( "Camera projection: %s\n", mat4_to_string(&camera->projection) );
	#endif
	return mat4_mult_matrix( &camera->projection_matrix, &camera->model_matrix );
}

vec3_t camera_forward_vector( const camera_t* camera )
{
	/*
	 * We take 4x4 orientation matrix and transpose it
	 * to get the inverse.  Then we create a 3x3 matrix
	 * of the orientation sub-matrix.
	 *
	 * |00 04 08 12|       |00 01 02 03|       |00 01 02|
	 * |01 05 09 13|  -->  |04 05 06 07|  -->  |04 05 06|
	 * |02 06 10 14|       |08 09 10 11|       |08 09 10|
	 * |03 07 11 15|       |12 13 14 15|
	 */
	const float* orientation_submatrix = camera->orientation_matrix.m;
	mat3_t orientation_inverse_matrix = MAT3(
		orientation_submatrix[ 0], orientation_submatrix[ 4], orientation_submatrix[ 8],
		orientation_submatrix[ 1], orientation_submatrix[ 5], orientation_submatrix[ 9],
		orientation_submatrix[ 2], orientation_submatrix[ 6], orientation_submatrix[10]
	);
	return mat3_mult_vector( &orientation_inverse_matrix, &VEC3_ZUNIT );
}

vec3_t camera_up_vector( const camera_t* camera )
{
	/*
	 * We take 4x4 orientation matrix and transpose it
	 * to get the inverse.  Then we create a 3x3 matrix
	 * of the orientation sub-matrix.
	 *
	 * |00 04 08 12|       |00 01 02 03|       |00 01 02|
	 * |01 05 09 13|  -->  |04 05 06 07|  -->  |04 05 06|
	 * |02 06 10 14|       |08 09 10 11|       |08 09 10|
	 * |03 07 11 15|       |12 13 14 15|
	 */
	const float* orientation_submatrix = camera->orientation_matrix.m;
	mat3_t orientation_inverse_matrix = MAT3(
		orientation_submatrix[ 0], orientation_submatrix[ 4], orientation_submatrix[ 8],
		orientation_submatrix[ 1], orientation_submatrix[ 5], orientation_submatrix[ 9],
		orientation_submatrix[ 2], orientation_submatrix[ 6], orientation_submatrix[10]
	);
	return mat3_mult_vector( &orientation_inverse_matrix, &VEC3_YUNIT );
}

vec3_t camera_side_vector( const camera_t* camera )
{
	/*
	 * We take 4x4 orientation matrix and transpose it
	 * to get the inverse.  Then we create a 3x3 matrix
	 * of the orientation sub-matrix.
	 *
	 * |00 04 08 12|       |00 01 02 03|       |00 01 02|
	 * |01 05 09 13|  -->  |04 05 06 07|  -->  |04 05 06|
	 * |02 06 10 14|       |08 09 10 11|       |08 09 10|
	 * |03 07 11 15|       |12 13 14 15|
	 */
	const float* orientation_submatrix = camera->orientation_matrix.m;
	mat3_t orientation_inverse_matrix = MAT3(
		orientation_submatrix[ 0], orientation_submatrix[ 4], orientation_submatrix[ 8],
		orientation_submatrix[ 1], orientation_submatrix[ 5], orientation_submatrix[ 9],
		orientation_submatrix[ 2], orientation_submatrix[ 6], orientation_submatrix[10]
	);
	return mat3_mult_vector( &orientation_inverse_matrix, &VEC3_XUNIT );
}

void camera_set_perspective( camera_t* camera, int screen_width, int screen_height, GLfloat near, GLfloat far, GLfloat fov )
{
	const GLfloat aspect = ((GLfloat)screen_width) / screen_height;
	camera->projection_matrix = perspective( fov * RADIANS_PER_DEGREE, aspect, near, far );
}

void camera_set_position( camera_t* camera, const pt3_t* position )
{
	assert( camera );
	assert( position );
	camera->position = *position;
}

void camera_offset_orientation( camera_t* camera, GLfloat xangle, GLfloat yangle )
{
	assert( camera );
    camera->xangle += xangle;
    camera->yangle += yangle;
    camera->yangle = clampf( camera->yangle, -HALF_PI, HALF_PI );
}

void camera_update( camera_t* camera, GLfloat delta )
{
	assert( camera );

    mat4_t xrotation = mat4_from_axis3_angle( &VEC3_YUNIT, camera->xangle );
    mat4_t yrotation = mat4_from_axis3_angle( &VEC3_XUNIT, camera->yangle );

    camera->orientation_matrix = mat4_mult_matrix( &xrotation, &MAT4_IDENTITY );
    camera->orientation_matrix = mat4_mult_matrix( &yrotation, &camera->orientation_matrix );

	mat4_t translation = translate( &camera->position );
	camera->model_matrix = mat4_mult_matrix( &translation, &camera->orientation_matrix );
}

