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
#ifndef _MAT3_H_
#define _MAT3_H_
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

#include <vec3.h>

#if defined(MAT3_USE_LONG_DOUBLE)
	#ifndef SCALAR_T
	#define SCALAR_T 
	typedef long double scaler_t;
	#endif
	#ifndef SCALAR_EPSILON
	#define SCALAR_EPSILON LDBL_EPSILON
	#endif
#elif defined(MAT3_USE_DOUBLE)
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
 * Three Dimensional Matrices
 */
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

extern const mat3_t MAT3_IDENITY;
extern const mat3_t MAT3_ZERO;

void          mat3_identity    ( mat3_t* m );
void          mat3_zero        ( mat3_t* m );
scaler_t      mat3_determinant ( const mat3_t* m );
const mat3_t  mat3_mult_matrix ( const mat3_t* __restrict a, const mat3_t* __restrict b );
const vec3_t  mat3_mult_vector ( const mat3_t* __restrict m, const vec3_t* __restrict v );
void          mat3_invert      ( mat3_t* m );
void          mat3_transpose   ( mat3_t* m );
void          mat3_adjoint     ( mat3_t* m );
const char*   mat3_to_string   ( const mat3_t* m );

#define mat3_x_vector( p_m )   ((vec3_t*) &(p_m)->x)
#define mat3_y_vector( p_m )   ((vec3_t*) &(p_m)->y)
#define mat3_z_vector( p_m )   ((vec3_t*) &(p_m)->z)


#ifdef __cplusplus
} /* C linkage */
#endif
#endif /* _MAT3_H_ */