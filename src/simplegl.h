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
#ifndef _SIMPLEGL_H_
#define _SIMPLEGL_H_
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #ifdef __restrict
    #undef __restrict
    #define __restrict restrict
    #endif
    #ifdef __inline
    #undef __inline
    #define __inline inline
    #endif
#else /* Not C99 */
    #ifdef __restrict
    #undef __restrict
    #define __restrict
    #endif
    #ifdef __inline
    #undef __inline
    #define __inline
    #endif
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <vec2.h>
#include <vec3.h>
#include <vec4.h>
#include <mat2.h>
#include <mat3.h>
#include <mat4.h>


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



/*
 * Shaders
 */
typedef struct shader_info {
	GLenum type;
	char*  filename;
} shader_info_t;

GLboolean     simplegl_program_from_shaders      ( GLuint* program, const shader_info_t* shaders, GLsizei count );
GLboolean     simplegl_shader_create_from_source ( GLuint* shader, GLenum type, const GLchar* source );
const GLchar* simplegl_shader_log                ( GLuint shader );
GLboolean     simplegl_program_create            ( GLuint* program, GLuint *shaders, GLsizei shader_count, GLboolean mark_shaders_for_deletion );
const GLchar* simplegl_program_log               ( GLuint program );

#ifdef __cplusplus
} /* C linkage */
#endif
#endif /* _SIMPLEGL_H_ */
