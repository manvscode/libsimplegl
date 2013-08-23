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
#ifndef _VEC3_H_
#define _VEC3_H_
#include <float.h>
#include <limits.h>
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#ifdef __restrict
#undef __restrict
#define __restrict restrict
#endif
#ifdef __inline
#undef __inline
#define __inline inline
#endif
#else
#define bool int
#define true 1
#define false 0
#ifdef __restrict
#undef __restrict
#define __restrict
#endif
#ifdef __inline
#undef __inline
#define __inline
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif

#if defined(VEC3_USE_LONG_DOUBLE)
	#ifndef SCALAR_T
	#define SCALAR_T 
	typedef long double scaler_t;
	#endif
	#ifndef SCALAR_EPSILON
	#define SCALAR_EPSILON LDBL_EPSILON
	#endif
#elif defined(VEC3_USE_DOUBLE)
	#ifndef SCALAR_T
	#define SCALAR_T 
	typedef double scaler_t;
	#endif
	#ifndef SCALAR_EPSILON
	#define SCALAR_EPSILON DBL_EPSILON
	#endif
#else /* default: use float */
	#ifndef SCALAR_T
	#define SCALAR_T 
	typedef float scaler_t;
	#endif
	#ifndef SCALAR_EPSILON
	#define SCALAR_EPSILON FLT_EPSILON
	#endif
#endif

/*
 * Three Dimensional Vectors
 */
typedef struct vec3 {
    union { /* allowed in C11 */
        struct {
            scaler_t x;
            scaler_t y;
            scaler_t z;
        };
        scaler_t v[ 3 ];
    };
} vec3_t;

extern const vec3_t VEC3_ZERO;
extern const vec3_t VEC3_XUNIT;
extern const vec3_t VEC3_YUNIT;
extern const vec3_t VEC3_ZUNIT;

/* |a|
 * |b|
 * |c|
 */
#define VEC3_VECTOR(a,b,c)  { .v = { a, b, c } }

vec3_t      vec3_add           ( const vec3_t* a, const vec3_t* b );
vec3_t      vec3_subtract      ( const vec3_t* a, const vec3_t* b );
vec3_t      vec3_multiply      ( const vec3_t* v, scaler_t s );
void        vec3_scale         ( vec3_t* v, scaler_t s );
scaler_t    vec3_dot_product   ( const vec3_t* a, const vec3_t* b );
vec3_t      vec3_cross_product ( const vec3_t* a, const vec3_t* b );
scaler_t    vec3_magnitude     ( const vec3_t* v );
scaler_t    vec3_distance      ( const vec3_t* a, const vec3_t* b );
scaler_t    vec3_angle         ( const vec3_t* a, const vec3_t* b ); /* in radians */
void        vec3_normalize     ( vec3_t* v );
bool        vec3_is_normalized ( const vec3_t* v );
void        vec3_negate        ( vec3_t* v );
bool        vec3_compare       ( const vec3_t* a, const vec3_t* b );
void        vec3_zero          ( vec3_t* v );
const char* vec3_to_string     ( const vec3_t* v ); /* not thread safe */


#ifdef __cplusplus
} /* C linkage */
#endif
#endif /* _VEC3_H_ */
