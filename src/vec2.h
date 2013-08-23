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
#ifndef _VEC2_H_
#define _VEC2_H_
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

#if defined(VEC2_USE_LONG_DOUBLE)
	#ifndef SCALAR_T
	#define SCALAR_T 
	typedef long double scaler_t;
	#endif
	#ifndef SCALAR_EPSILON
	#define SCALAR_EPSILON LDBL_EPSILON
	#endif
#elif defined(VEC2_USE_DOUBLE)
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
 * Two Dimensional Vectors
 */
typedef struct vec2 {
	union { /* allowed in C11 */
		struct {
			scaler_t x;
			scaler_t y;
		};
		scaler_t v[ 2 ];
	};
} vec2_t;


extern const vec2_t VEC2_ZERO;
extern const vec2_t VEC2_XUNIT;
extern const vec2_t VEC2_YUNIT;

/* |a|
 * |b|
 */
#define VEC2_VECTOR(a,b)  { .v = { a, b } }

vec2_t      vec2_add           ( const vec2_t* a, const vec2_t* b );
vec2_t      vec2_subtract      ( const vec2_t* a, const vec2_t* b );
vec2_t      vec2_multiply      ( const vec2_t* v, scaler_t s );
void        vec2_scale         ( vec2_t* v, scaler_t s );
scaler_t    vec2_dot_product   ( const vec2_t* a, const vec2_t* b );
vec2_t      vec2_cross_product ( const vec2_t* v );
scaler_t    vec2_determinant   ( const vec2_t* a, const vec2_t* b );
scaler_t    vec2_magnitude     ( const vec2_t* v );
scaler_t    vec2_distance      ( const vec2_t* a, const vec2_t* b );
scaler_t    vec2_angle         ( const vec2_t* a, const vec2_t* b ); /* in radians */
void        vec2_normalize     ( vec2_t* v );
bool        vec2_is_normalized ( const vec2_t* v );
void        vec2_negate        ( vec2_t* v );
bool        vec2_compare       ( const vec2_t* a, const vec2_t* b );
void        vec2_zero          ( vec2_t* v );
const char* vec2_to_string     ( const vec2_t* v ); /* not thread safe */

#define     vec2_area          vec2_determinant

#ifdef __cplusplus
} /* C linkage */
#endif
#endif /* _VEC2_H_ */
