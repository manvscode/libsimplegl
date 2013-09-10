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
#include <math.h>
#include <string.h>
#include <assert.h>
#include "mat3.h"

const mat3_t MAT3_IDENTITY = { .m = {
					1.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 1.0f
				}};

const mat3_t MAT3_ZERO = { .m = {
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f
				}};

void mat3_identity( mat3_t* m )
{
	*m = MAT3_IDENTITY;
}

void mat3_zero( mat3_t* m )
{
	*m = MAT3_ZERO;
}

scaler_t mat3_determinant( const mat3_t* m )
{
	scaler_t det = m->m[ 0 ] * m->m[ 4 ] * m->m[ 8 ] +
                   m->m[ 3 ] * m->m[ 7 ] * m->m[ 2 ] +
                   m->m[ 6 ] * m->m[ 1 ] * m->m[ 5 ] -
                   m->m[ 6 ] * m->m[ 4 ] * m->m[ 2 ] -
                   m->m[ 3 ] * m->m[ 1 ] * m->m[ 8 ] -
                   m->m[ 0 ] * m->m[ 7 ] * m->m[ 5 ];

	return det;
}

mat3_t mat3_mult_matrix( const mat3_t* __restrict a, const mat3_t* __restrict b )
{
	mat3_t result;
    assert( a && b );

	// |(A0*B0 + A3*B1 + A6*B2) (A0*B3 + A3*B4 + A6*B5) (A0*B6 + A3*B7 + A6*B8)|   |A0 A3 A6|   |B0 B3 B6|
	// |(A1*B0 + A4*B1 + A7*B2) (A1*B3 + A4*B4 + A7*B5) (A1*B6 + A4*B7 + A7*B8)| = |A1 A4 A7| * |B1 B4 B7|
	// |(A2*B0 + A5*B1 + A8*B2) (A2*B3 + A5*B4 + A8*B5) (A2*B6 + A5*B7 + A8*B8)|   |A2 A5 A8|   |B2 B5 B8|

	result.m[ 0 ] = a->m[ 0 ] * b->m[ 0 ] + a->m[ 3 ] * b->m[ 1 ] + a->m[ 6 ] * b->m[ 2 ];
	result.m[ 1 ] = a->m[ 1 ] * b->m[ 0 ] + a->m[ 4 ] * b->m[ 1 ] + a->m[ 7 ] * b->m[ 2 ];
	result.m[ 2 ] = a->m[ 2 ] * b->m[ 0 ] + a->m[ 5 ] * b->m[ 1 ] + a->m[ 8 ] * b->m[ 2 ];

	result.m[ 3 ] = a->m[ 0 ] * b->m[ 3 ] + a->m[ 3 ] * b->m[ 4 ] + a->m[ 6 ] * b->m[ 5 ];
	result.m[ 4 ] = a->m[ 1 ] * b->m[ 3 ] + a->m[ 4 ] * b->m[ 4 ] + a->m[ 7 ] * b->m[ 5 ];
	result.m[ 5 ] = a->m[ 2 ] * b->m[ 3 ] + a->m[ 5 ] * b->m[ 4 ] + a->m[ 8 ] * b->m[ 5 ];

	result.m[ 6 ] = a->m[ 0 ] * b->m[ 6 ] + a->m[ 3 ] * b->m[ 7 ] + a->m[ 6 ] * b->m[ 8 ];
	result.m[ 7 ] = a->m[ 1 ] * b->m[ 6 ] + a->m[ 4 ] * b->m[ 7 ] + a->m[ 7 ] * b->m[ 8 ];
	result.m[ 8 ] = a->m[ 2 ] * b->m[ 6 ] + a->m[ 5 ] * b->m[ 7 ] + a->m[ 8 ] * b->m[ 8 ];

	return result;
}

vec3_t mat3_mult_vector( const mat3_t* __restrict m, const vec3_t* __restrict v )
{
    vec3_t result;
    assert( m && v );

	// |m0x + m1y + m2z|   | m0 m3 m6 |    |x|
	// |m3x + m4y + m5z| = | m1 m4 m7 | *  |y|
	// |m6x + m7y + m8z| = | m2 m5 m8 |    |z|
	result.x = m->m[ 0 ] * v->x  +  m->m[ 3 ] * v->y  +  m->m[ 6 ] * v->z;
	result.y = m->m[ 1 ] * v->x  +  m->m[ 4 ] * v->y  +  m->m[ 7 ] * v->z;
	result.z = m->m[ 2 ] * v->x  +  m->m[ 5 ] * v->y  +  m->m[ 8 ] * v->z;

    return result;
}

bool mat3_invert( mat3_t* m )
{
    assert( false );

	scaler_t det = mat3_determinant( m );

	if( det > SCALAR_EPSILON ) // testing if not zero
	{
		mat3_adjoint( m );

		m->m[ 0 ] /= det;
		m->m[ 1 ] /= det;
		m->m[ 2 ] /= det;
		m->m[ 3 ] /= det;
		m->m[ 4 ] /= det;
		m->m[ 5 ] /= det;
		m->m[ 6 ] /= det;
		m->m[ 7 ] /= det;
		m->m[ 8 ] /= det;

		return true;
	}

	return false;
}

void mat3_transpose( mat3_t* m )
{
    assert( m );

	scaler_t tmp1 = m->m[ 1 ];
	scaler_t tmp2 = m->m[ 2 ];
	scaler_t tmp3 = m->m[ 5 ];

	m->m[ 1 ] = m->m[ 3 ];
	m->m[ 2 ] = m->m[ 6 ];
	m->m[ 5 ] = m->m[ 7 ];

	m->m[ 3 ] = tmp1;
	m->m[ 6 ] = tmp2;
	m->m[ 7 ] = tmp3;
}

mat3_t mat3_cofactor( mat3_t* m )
{
	mat3_t cofactor;
	assert( m );
	// | m0 m3 m6 |
	// | m1 m4 m7 |
	// | m2 m5 m8 |

	cofactor.m[ 0 ] = +(m->m[4] * m->m[8] - m->m[5] * m->m[7]);
	cofactor.m[ 1 ] = -(m->m[3] * m->m[8] - m->m[5] * m->m[6]);
	cofactor.m[ 2 ] = +(m->m[3] * m->m[7] - m->m[4] * m->m[6]);

	cofactor.m[ 3 ] = -(m->m[1] * m->m[8] - m->m[2] * m->m[7]);
	cofactor.m[ 4 ] = +(m->m[0] * m->m[8] - m->m[2] * m->m[6]);
	cofactor.m[ 5 ] = -(m->m[0] * m->m[7] - m->m[1] * m->m[6]);

	cofactor.m[ 6 ] = +(m->m[1] * m->m[5] - m->m[2] * m->m[4]);
	cofactor.m[ 7 ] = -(m->m[0] * m->m[5] - m->m[2] * m->m[3]);
	cofactor.m[ 8 ] = +(m->m[0] * m->m[4] - m->m[1] * m->m[3]);

	return cofactor;
}

void mat3_adjoint( mat3_t* m )
{
	mat3_t cofactor_matrix = mat3_cofactor( m );
	mat3_transpose( &cofactor_matrix );
	*m = cofactor_matrix;
}

const char* mat3_to_string( const mat3_t* m )
{
	static char string_buffer[ 128 ];
	snprintf( string_buffer, sizeof(string_buffer) - 1,
		"|%-5.1f %-5.1f %-5.1f|\n"
		"|%-5.1f %-5.1f %-5.1f|\n"
		"|%-5.1f %-5.1f %-5.1f|\n",
		m->m[0], m->m[3], m->m[6],
		m->m[1], m->m[4], m->m[7],
		m->m[2], m->m[5], m->m[8]
 	);
	string_buffer[ sizeof(string_buffer) - 1 ] = '\0';
	return string_buffer;
}

#if 0
const vec3_t* mat3_x_vector( const mat3_t* m )
{
    return (vec3_t*) &m->x;
}

const vec3_t* mat3_y_vector( const mat3_t* m )
{
    return (vec3_t*) &m->y;
}

const vec3_t* mat3_z_vector( const mat3_t* m )
{
    return (vec3_t*) &m->z;
}
#endif
