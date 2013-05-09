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

const mat3_t MAT3_IDENITY = { .m = {
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
	*m = MAT3_IDENITY;
}

void mat3_zero( mat3_t* m )
{
	*m = MAT3_ZERO;
}

scaler_t mat3_determinant( const mat3_t* m )
{
	return 0;
}

const mat3_t mat3_mult_matrix( const mat3_t* __restrict a, const mat3_t* __restrict b )
{
	return MAT3_IDENITY;
}

const vec3_t mat3_mult_vector( const mat3_t* __restrict m, const vec3_t* __restrict v )
{
    vec3_t result;
    // TODO: Implement this!
    assert( false );
    return result;
}

void mat3_invert( mat3_t* m )
{
    // TODO: Implement this!
    assert( false );
}

void mat3_transpose( mat3_t* m )
{
}

void mat3_adjoint( mat3_t* m )
{
}

const char* mat3_to_string( const mat3_t* m )
{
	static char string_buffer[ 128 ];
	snprintf( string_buffer, sizeof(string_buffer) - 1,
		"|%-6.1f %-6.1f %6.1f|\n"
		"|%-6.1f %-6.1f %6.1f|\n"
		"|%-6.1f %-6.1f %6.1f|\n",
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
