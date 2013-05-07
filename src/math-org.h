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
#ifndef _SIMPLEGL_MATH_H_
#define _SIMPLEGL_MATH_H_
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

#if defined(SIMPLEGL_MATH_USE_LONG_DOUBLE)
	#ifndef SCALAR_T
	#define SCALAR_T 
	typedef long double scaler_t;
	#endif
	#ifndef SCALAR_EPSILON
	#define SCALAR_EPSILON LDBL_EPSILON
	#endif
#elif defined(SIMPLEGL_MATH_USE_DOUBLE)
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

#include <vec3.h>
#include <vec4.h>

/*
 * Three Dimensional Matrices
typedef struct mat3 {
    union { 
        struct {
            vec3_t x;
            vec3_t y;
            vec3_t z;
        };
        scaler_t m[ 9 ];
    };
} mat3_t;

const vec3_t* mat3_x_vector ( const mat3_t* m );
const vec3_t* mat3_y_vector ( const mat3_t* m );
const vec3_t* mat3_z_vector ( const mat3_t* m );
const vec3_t  mat3_multiply ( const mat3_t* m, const vec3_t* v );
void          mat3_invert   ( mat3_t* m );
void          mat3_transpose( mat3_t* m );
*/

/*
 * Four Dimensional Matrices
typedef struct mat4 {
    union { // allowed in C11
        struct {
            vec4_t x;
            vec4_t y;
            vec4_t z;
            vec4_t w;
        };
        scaler_t m[ 16 ];
    };
} mat4_t;
*/

#ifdef __cplusplus
} /* C linkage */
#endif
#endif /* _SIMPLEGL_MATH_H_ */
