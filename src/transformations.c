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
#include <stdarg.h>
#include <assert.h>
#include "math.h"
#include "simplegl.h"

static const mat4_t changed_handedness = MAT4_LITERAL(
	// convert from our coordinate system (looking down X)
	// to OpenGL's coordinate system (looking down -Z)
	0, 0, -1, 0,
	-1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 1
);

mat4_t translate( const vec3_t* t )
{
	return MAT4_LITERAL(
		 1.0,  0.0,  0.0,  0.0,
		 0.0,  1.0,  0.0,  0.0,
		 0.0,  0.0,  1.0,  0.0,
	 	t->x, t->y, t->z,  1.0
	);
}

mat4_t scale( const vec3_t* s )
{
	return MAT4_LITERAL(
		s->x,  0.0,  0.0,  0.0,
		 0.0, s->y,  0.0,  0.0,
	 	 0.0,  0.0, s->z,  0.0,
	 	 0.0,  0.0,  0.0,  1.0
	);
}

mat4_t uniform_scale( GLdouble s )
{
	return MAT4_LITERAL(
		  s, 0.0, 0.0, 0.0,
		0.0,   s, 0.0, 0.0,
	 	0.0, 0.0,   s, 0.0,
	 	0.0, 0.0, 0.0, 1.0
	);
}

mat4_t rotate_x( GLdouble a )
{
	GLdouble s = sin( a );
	GLdouble c = cos( a );

	return MAT4_LITERAL(
		1.0, 0.0, 0.0, 0.0,
		0.0,   c,  -s, 0.0,
	 	0.0,   s,   c, 0.0,
	 	0.0, 0.0, 0.0, 1.0
	);
}

mat4_t rotate_y( GLdouble a )
{
	GLdouble s = sin( a );
	GLdouble c = cos( a );

	return MAT4_LITERAL(
		  c, 0.0,   s, 0.0,
		0.0, 1.0, 0.0, 0.0,
	 	 -s, 0.0,   c, 0.0,
	 	0.0, 0.0, 0.0, 1.0
	);
}

mat4_t rotate_z( GLdouble a )
{
	GLdouble s = sin( a );
	GLdouble c = cos( a );

	return MAT4_LITERAL(
		  c,  -s, 0.0, 0.0,
		  s,   c, 0.0, 0.0,
	 	0.0, 0.0, 1.0, 0.0,
	 	0.0, 0.0, 0.0, 1.0
	);
}

mat4_t rotate_xyz( const char* order, ... )
{
	va_list list;
	va_start( list, order );
	mat4_t result = MAT4_IDENTITY;

	while( *order )
	{
		switch( *order++ )
		{
			case 'x':
			case 'X':
			{
				GLdouble angle = va_arg( list, GLdouble );
				mat4_t r = rotate_x( angle );
				result = mat4_mult_matrix( &result, &r );
				break;
			}
			case 'y':
			case 'Y':
			{
				GLdouble angle = va_arg( list, GLdouble );
				mat4_t r = rotate_y( angle );
				result = mat4_mult_matrix( &result, &r );
				break;
			}
			case 'z':
			case 'Z':
			{
				GLdouble angle = va_arg( list, GLdouble );
				mat4_t r = rotate_z( angle );
				result = mat4_mult_matrix( &result, &r );
				break;
			}
			default:
				break;
		}
	}

	va_end( list );
	return result;
}

/*
mat4_t rotate_xyz( GLdouble xa, GLdouble ya, GLdouble za )
{
	mat4_t result = MAT4_IDENTITY;

	if( abs(xa) < SCALAR_EPSILON )
	{
		mat4_t rx = rotate_x( xa );
		mat4_mult_matrix( result, &rx );
	}

	if( abs(ya) < SCALAR_EPSILON )
	{
		mat4_t ry = rotate_y( ya );
		mat4_mult_matrix( result, &ry );
	}

	if( abs(za) < SCALAR_EPSILON )
	{
		mat4_t rz = rotate_z( za );
	}
}
*/

mat4_t orientation( vec3_t* f, vec3_t* l, vec3_t* u )
{
	vec3_normalize( f );
	vec3_normalize( l );
	vec3_normalize( u );

	mat4_t transform = MAT4_LITERAL(
		l->x, l->y, l->z, 0.0,
		u->x, u->y, u->z, 0.0,
		f->x, f->y, f->z, 0.0,
		0.0,   0.0,  0.0, 1.0
	);
	return transform;
}
