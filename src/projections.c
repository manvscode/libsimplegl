/* Copyright (C) 2013-2014 by Joseph A. Marrero, http://www.manvscode.com/
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
#include <math.h>
#include <assert.h>
#include <m3d/mathematics.h>
#include "simplegl.h"


mat4_t orthographic( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far )
{
	return MAT4(
		2.0 / (right - left)          , 0.0                           ,  0.0                      , 0.0,
		0.0                           , 2.0 / (top - bottom)          ,  0.0                      , 0.0,
		0.0                           , 0.0                           , -2.0 / (far - near)       , 0.0,
		-(right + left)/(right - left), -(top + bottom)/(top - bottom), -(far + near)/(far - near), 1.0
	);
}

mat4_t frustum( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far )
{
	GLfloat A = 2.0 * near / (right - left);
	GLfloat B = (right + left) / (right - left);
	GLfloat C = 2.0 * near / (top - bottom);
	GLfloat D = (top + bottom) / (top - bottom);
	GLfloat E = -(far + near) / (far - near);
	GLfloat F = -(2.0 * far * near) / (far - near);

	return MAT4(
		  A, 0.0,   B, 0.0,
		0.0,   C,   D, 0.0,
		0.0, 0.0,   E,   F,
		0.0, 0.0,-1.0, 0.0
	);
}


mat4_t perspective( GLfloat fov /* in radians */, GLfloat aspect, GLfloat near, GLfloat far )
{
	GLfloat A = 1.0 / tan(fov * 0.5);
	GLfloat B = -far / (far - near);
	GLfloat C = -(far * near)/ (far - near);

	return MAT4(
	    A/aspect, 0.0, 0.0, 0.0,
	         0.0,   A, 0.0, 0.0,
	         0.0, 0.0,   B,-1.0,
	         0.0, 0.0,   C, 0.0
	);
}


vec4_t viewport_unproject( const vec2_t* point, const mat4_t* projection, const mat4_t* model )
{
	GLint viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT, viewport );

	/* Convert to normalized device coordinates */
	vec4_t normalized_device_coordinate = VEC4(
		(point->x - viewport[0]) * (2.0f / viewport[2]) - 1.0f,
		(point->y - viewport[1]) * (2.0f / viewport[3]) - 1.0f,
		0.0f,
		1.0f
	);

	mat4_t inv_projmodel = mat4_mult_matrix( projection, model );
	mat4_invert( &inv_projmodel );

	return mat4_mult_vector( &inv_projmodel, &normalized_device_coordinate );
}

vec2_t viewport_project( const vec4_t* point, const mat4_t* projection, const mat4_t* model )
{
	GLint viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT, viewport );

	mat4_t projmodel = mat4_mult_matrix( projection, model );
	vec4_t pt = mat4_mult_vector( &projmodel, point );

	return VEC2(
		(1.0f + pt.x) * (viewport[2] / 2.0f) + viewport[0],
		(1.0f + pt.y) * (viewport[3] / 2.0f) + viewport[1]
	);
}
