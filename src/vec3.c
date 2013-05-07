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
#include <float.h>
#include <math.h>
#include <limits.h>
#include "vec3.h"

static __inline scaler_t fast_inverse_sqrt( scaler_t number );

const vec3_t VEC3_ZERO  = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
const vec3_t VEC3_XUNIT = { .x = 1.0f, .y = 0.0f, .z = 0.0f };
const vec3_t VEC3_YUNIT = { .x = 0.0f, .y = 1.0f, .z = 0.0f };
const vec3_t VEC3_ZUNIT = { .x = 0.0f, .y = 0.0f, .z = 1.0f };

vec3_t vec3_add( const vec3_t* a, const vec3_t* b )
{
    vec3_t result;
    result.x = a->x + b->x;
    result.y = a->y + b->y;
    result.z = a->z + b->z;
    return result;
}

vec3_t vec3_subtract( const vec3_t* a, const vec3_t* b )
{
    vec3_t result;
    result.x = a->x - b->x;
    result.y = a->y - b->y;
    result.z = a->z - b->z;
    return result;
}

vec3_t vec3_multiply( const vec3_t* v, scaler_t s )
{
    vec3_t result;
    result.x = v->x * s;
    result.y = v->y * s;
    result.z = v->z * s;
    return result;
}

void vec3_scale( vec3_t* v, scaler_t s )
{
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

scaler_t vec3_dot_product( const vec3_t* a, const vec3_t* b )
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

vec3_t vec3_cross_product( const vec3_t* a, const vec3_t* b )
{
    vec3_t result;
    result.x = a->y * b->z - a->z * b->y;
    result.y = a->z * b->x - a->x * b->z;
    result.z = a->x * b->y - a->y * b->x;
    return result;
}

scaler_t vec3_magnitude( const vec3_t* v )
{
	#if defined(VEC3_USE_LONG_DOUBLE)
    return sqrtl( v->x * v->x + v->y * v->y + v->z * v->z );
	#elif defined(VEC3_USE_DOUBLE)
    return sqrt( v->x * v->x + v->y * v->y + v->z * v->z );
	#else /* default: use float */
    return sqrtf( v->x * v->x + v->y * v->y + v->z * v->z );
	#endif
}

scaler_t vec3_angle( const vec3_t* a, const vec3_t* b ) /* in radians */
{
    scaler_t dot_product = vec3_dot_product( a, b );
    scaler_t a_length    = vec3_magnitude( a );
    scaler_t b_length    = vec3_magnitude( b );

	#if defined(VEC3_USE_LONG_DOUBLE)
    return acosl( dot_product / ( a_length * b_length ) );
	#elif defined(VEC3_USE_DOUBLE)
    return acos( dot_product / ( a_length * b_length ) );
	#else /* default: use float */
    return acosf( dot_product / ( a_length * b_length ) );
	#endif
}

void vec3_normalize( vec3_t* v )
{
	#if 0
    scaler_t length = vec3_magnitude( v );
    v->x /= length;
    v->y /= length;
    v->z /= length;
	#else
    scaler_t inverse_length = fast_inverse_sqrt( v->x * v->x + v->y * v->y + v->z * v->z );
    v->x *= inverse_length;
    v->y *= inverse_length;
    v->z *= inverse_length;
	#endif
}

bool vec3_is_normalized( const vec3_t* v )
{
	#if defined(VEC3_USE_LONG_DOUBLE)
    return (fabsl(v->x - 1.0) < SCALAR_EPSILON) &&
           (fabsl(v->y - 1.0) < SCALAR_EPSILON) &&
           (fabsl(v->z - 1.0) < SCALAR_EPSILON);
	#elif defined(VEC3_USE_DOUBLE)
    return (fabs(v->x - 1.0) < SCALAR_EPSILON) &&
           (fabs(v->y - 1.0) < SCALAR_EPSILON) &&
           (fabs(v->z - 1.0) < SCALAR_EPSILON);
	#else /* default: use float */
    return (fabsf(v->x - 1.0f) < SCALAR_EPSILON) &&
           (fabsf(v->y - 1.0f) < SCALAR_EPSILON) &&
           (fabsf(v->z - 1.0f) < SCALAR_EPSILON);
	#endif
}

void vec3_negate( vec3_t* v )
{
    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;
}

bool vec3_compare( const vec3_t* a, const vec3_t* b )
{
	#if defined(VEC3_USE_LONG_DOUBLE)
    return (fabsl(a->x - b->x) < SCALAR_EPSILON) &&
           (fabsl(a->y - b->y) < SCALAR_EPSILON) &&
           (fabsl(a->z - b->z) < SCALAR_EPSILON);
	#elif defined(VEC3_USE_DOUBLE)
    return (fabs(a->x - b->x) < SCALAR_EPSILON) &&
           (fabs(a->y - b->y) < SCALAR_EPSILON) &&
           (fabs(a->z - b->z) < SCALAR_EPSILON);
	#else /* default: use float */
    return (fabsf(a->x - b->x) < SCALAR_EPSILON) &&
           (fabsf(a->y - b->y) < SCALAR_EPSILON) &&
           (fabsf(a->z - b->z) < SCALAR_EPSILON);
	#endif
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
