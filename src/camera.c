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

struct camera {
	vec3_t position;

	mat4_t projection_matrix;
	mat4_t model_matrix;
    mat4_t orientation_matrix;
};


camera_t* camera_create( int screen_width, int screen_height, GLfloat near, GLfloat far, const pt3_t* position )
{
	camera_t* c = malloc( sizeof(camera_t) );

	if( c )
	{
		const GLfloat aspect = ((GLfloat)screen_width) / screen_height;

		c->position           = position ? *position : VEC3_ZERO;
		c->projection_matrix  = perspective( 45.0 * RADIANS_PER_DEGREE, aspect, near, far );
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

void camera_set_position( camera_t* camera, const pt3_t* position )
{
	assert( camera );
	assert( position );

	camera->position        = *position;
	vec3_negate( &camera->position );
}

void camera_offset_orientation( camera_t* camera, GLfloat xangle, GLfloat yangle )
{
	assert( camera );



	//vec4_t* forward = mat4_z_vector( &camera->orientation_matrix );
	//vec3_t side_axis = vec3_cross_product( forward, &VEC3_YUNIT );


    //xangle = fmodf(xangle, TWO_PI);
    //fmodf can return negative values, but this will make them all positive
    //if(xangle < 0.0f)
        //xangle += TWO_PI;


    yangle = clampf( yangle, -HALF_PI, HALF_PI );



    mat4_t xrotation = mat4_from_axis3_angle( &VEC3_YUNIT, xangle );
    mat4_t yrotation = mat4_from_axis3_angle( &VEC3_XUNIT, yangle );

    camera->orientation_matrix = mat4_mult_matrix( &yrotation, &camera->orientation_matrix );
    camera->orientation_matrix = mat4_mult_matrix( &xrotation, &camera->orientation_matrix );


	//quat_t xrotation = quat_from_axis3_angle( &VEC3_YUNIT, xangle );
	//camera->target   = quat_rotate3( &xrotation, &camera->target );

	//quat_t yrotation = quat_from_axis3_angle( &side_axis, yangle );
	//camera->target   = quat_rotate3( &yrotation, &camera->target );
}

void camera_update( camera_t* camera, GLfloat delta )
{
	assert( camera );

    //mat4_t rotation = quat_to_mat4( &camera->orientation );
	//camera->orientation_matrix = look_at( &camera->position, &camera->target, &VEC3_YUNIT );

	mat4_t translation = translate( &camera->position );
	//translation.m[12] *= -1.0f;
	//translation.m[13] *= -1.0f;
	//translation.m[14] *= -1.0f;

	//return mat4_mult_matrix( &rotation, &translation );
	camera->model_matrix = mat4_mult_matrix( &translation, &camera->orientation_matrix );

}

mat4_t camera_matrix( const camera_t* camera )
{
	assert( camera );

	#ifdef SIMPLEGL_DEBUG
	//printf( "     Camera model: %s\n", mat4_to_string(&model) );
	//printf( "Camera projection: %s\n", mat4_to_string(&camera->projection) );
	#endif
	return mat4_mult_matrix( &camera->projection_matrix, &camera->model_matrix );
}
