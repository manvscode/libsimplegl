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
#include <float.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include "vec2.h"

static __inline scaler_t fast_inverse_sqrt( scaler_t number );

const vec2_t VEC2_ZERO  = { .x = 0.0f, .y = 0.0f };
const vec2_t VEC2_XUNIT = { .x = 1.0f, .y = 0.0f };
const vec2_t VEC2_YUNIT = { .x = 0.0f, .y = 1.0f };

vec2_t vec2_add( const vec2_t* a, const vec2_t* b )
{
    vec2_t result;
    result.x = a->x + b->x;
    result.y = a->y + b->y;
    return result;
}

vec2_t vec2_subtract( const vec2_t* a, const vec2_t* b )
{
    vec2_t result;
    result.x = a->x - b->x;
    result.y = a->y - b->y;
    return result;
}

vec2_t vec2_multiply( const vec2_t* v, scaler_t s )
{
    vec2_t result;
    result.x = v->x * s;
    result.y = v->y * s;
    return result;
}

void vec2_scale( vec2_t* v, scaler_t s )
{
    v->x *= s;
    v->y *= s;
}

scaler_t vec2_dot_product( const vec2_t* a, const vec2_t* b )
{
    return a->x * b->x + a->y * b->y;
}

vec2_t vec2_cross_product( const vec2_t* a, const vec2_t* b )
{
    vec2_t result;
    //result.x = a->y * b->z - a->z * b->y;
    //result.y = a->z * b->x - a->x * b->z;
    return result;
}

scaler_t vec2_magnitude( const vec2_t* v )
{
	#if defined(VEC3_USE_LONG_DOUBLE)
    return sqrtl( v->x * v->x + v->y * v->y );
	#elif defined(VEC3_USE_DOUBLE)
    return sqrt( v->x * v->x + v->y * v->y );
	#else /* default: use float */
    return sqrtf( v->x * v->x + v->y * v->y );
	#endif
}

scaler_t vec2_distance( const vec2_t* a, const vec2_t* b )
{
	#if defined(VEC3_USE_LONG_DOUBLE)
    return sqrtl(
		(a->x - b->x) * (a->x - b->x) +
		(a->y - b->y) * (a->y - b->y)
	);
	#elif defined(VEC3_USE_DOUBLE)
    return sqrt(
		(a->x - b->x) * (a->x - b->x) +
		(a->y - b->y) * (a->y - b->y)
	);
	#else /* default: use float */
    return sqrtf(
		(a->x - b->x) * (a->x - b->x) +
		(a->y - b->y) * (a->y - b->y)
	);
	#endif
}

scaler_t vec2_angle( const vec2_t* a, const vec2_t* b ) /* in radians */
{
    scaler_t dot_product = vec2_dot_product( a, b );
    scaler_t a_length    = vec2_magnitude( a );
    scaler_t b_length    = vec2_magnitude( b );

	#if defined(VEC3_USE_LONG_DOUBLE)
    return acosl( dot_product / ( a_length * b_length ) );
	#elif defined(VEC3_USE_DOUBLE)
    return acos( dot_product / ( a_length * b_length ) );
	#else /* default: use float */
    return acosf( dot_product / ( a_length * b_length ) );
	#endif
}

void vec2_normalize( vec2_t* v )
{
	#if 0
    scaler_t length = vec2_magnitude( v );
    v->x /= length;
    v->y /= length;
	#else
    scaler_t inverse_length = fast_inverse_sqrt( v->x * v->x + v->y * v->y );
    v->x *= inverse_length;
    v->y *= inverse_length;
	#endif
}

bool vec2_is_normalized( const vec2_t* v )
{
	#if defined(VEC3_USE_LONG_DOUBLE)
    return (fabsl(v->x - 1.0) < SCALAR_EPSILON) &&
           (fabsl(v->y - 1.0) < SCALAR_EPSILON);
	#elif defined(VEC3_USE_DOUBLE)
    return (fabs(v->x - 1.0) < SCALAR_EPSILON) &&
           (fabs(v->y - 1.0) < SCALAR_EPSILON);
	#else /* default: use float */
    return (fabsf(v->x - 1.0f) < SCALAR_EPSILON) &&
           (fabsf(v->y - 1.0f) < SCALAR_EPSILON);
	#endif
}

void vec2_negate( vec2_t* v )
{
    v->x = -v->x;
    v->y = -v->y;
}

bool vec2_compare( const vec2_t* a, const vec2_t* b )
{
	#if defined(VEC3_USE_LONG_DOUBLE)
    return (fabsl(a->x - b->x) < SCALAR_EPSILON) &&
           (fabsl(a->y - b->y) < SCALAR_EPSILON);
	#elif defined(VEC3_USE_DOUBLE)
    return (fabs(a->x - b->x) < SCALAR_EPSILON) &&
           (fabs(a->y - b->y) < SCALAR_EPSILON);
	#else /* default: use float */
    return (fabsf(a->x - b->x) < SCALAR_EPSILON) &&
           (fabsf(a->y - b->y) < SCALAR_EPSILON);
	#endif
}

void vec2_zero( vec2_t* v )
{
	memset( v, 0, sizeof(vec2_t) );
}

const char* vec2_to_string( const vec2_t* v ) /* not thread safe */
{
	static char string_buffer[ 128 ];
	snprintf( string_buffer, sizeof(string_buffer) - 1, "(%08.1f, %08.1f)", v->x, v->y );
	string_buffer[ sizeof(string_buffer) - 1 ] = '\0';
	return string_buffer;
}

scaler_t fast_inverse_sqrt( scaler_t number )
{
        long i;
        scaler_t x2, y;
        const scaler_t threehalfs = 1.5F;

        x2 = number * 0.5F;
        y  = number;
        i  = * (long *) &y;                       /* evil floating point bit level hacking */
        i  = 0x5f3759df - ( i >> 1 );               /* what the fuck? */
        y  = * (scaler_t *) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   /* 1st iteration */
		#if 0
        y  = y * ( threehalfs - ( x2 * y * y ) );   /* 2nd iteration, this can be removed */
		#endif

        return y;
}
