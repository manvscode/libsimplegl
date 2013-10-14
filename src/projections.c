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
#include <math.h>
#include <assert.h>
#include <lib3dmath/mathematics.h>
#include "simplegl.h"


mat4_t orthographic( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far )
{
	return MAT4_LITERAL(
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

	return MAT4_LITERAL(
		  A, 0.0,   B, 0.0,
		0.0,   C,   D, 0.0,
		0.0, 0.0,   E,   F,
		0.0, 0.0,-1.0, 0.0
	);
}


mat4_t perspective( GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far )
{
	GLfloat A = 1.0 / tan(fov * 0.5 * RADIANS_PER_DEGREE);
	GLfloat B = -far / (far - near);
	GLfloat C = -(far * near)/ (far - near);

	return MAT4_LITERAL(
		  A, 0.0, 0.0, 0.0,
		0.0,   A, 0.0, 0.0,
		0.0, 0.0,   B,-1.0,
		0.0, 0.0,   C, 0.0
	);
}

mat4_t look_at( const pt3_t* p, const pt3_t* t, const vec3_t* u )
{
	vec3_t z = VEC3_LITERAL( t->x - p->x, t->y - p->y, t->z - p->z );
	vec3_normalize( &z );

	vec3_t x = vec3_cross_product( z, *u );
	vec3_normalize( &x );

	vec3_t y = vec3_cross_product( z, x );
	vec3_normalize( &y );

	return MAT4_LITERAL(
		  x.x,   x.y,   x.z, 0.0, /* x-axis */
		  y.x,   y.y,   y.z, 0.0, /* y-axis */
		  z.x,   z.y,   z.z, 0.0, /* z-axis */
		-p->x, -p->y, -p->z, 1.0  /* translation */
	);
}

