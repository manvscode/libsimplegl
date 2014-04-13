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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "simplegl.h"

void dump_gl_info( void )
{
	const char* vendor       = (const char*) glGetString(GL_VENDOR);
	const char* renderer     = (const char*) glGetString(GL_RENDERER);
	const char* version      = (const char*) glGetString(GL_VERSION);
	const char* glsl_version = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);

	fprintf( stdout, "[GL] Vendor: %s\n", vendor ? vendor : "unknown" );
	fprintf( stdout, "[GL] Renderer: %s\n", renderer ? renderer : "unknown" );
	fprintf( stdout, "[GL] Version: %s\n", version ? version : "unknown" );
	fprintf( stdout, "[GL] Shading Language: %s\n", glsl_version ? glsl_version : "unknown" );

	#ifdef GL_NUM_EXTENSIONS
	fprintf( stdout, "[GL] Extensions: " );

	int extensionsCount = 0;
	assert(check_gl() == GL_NO_ERROR);
	glGetIntegerv( GL_NUM_EXTENSIONS, &extensionsCount );
	assert(check_gl() == GL_NO_ERROR);

	for( int i = 0; i < extensionsCount; i++ )
	{
		printf( "%s%s", glGetStringi( GL_EXTENSIONS, i ), i < (extensionsCount - 1) ? ", " : "\n" );
	}
	#endif
}

GLenum check_gl( void )
{
    GLenum error = glGetError();
	const char* error_str;

	switch( error )
	{
		case GL_INVALID_ENUM:
			error_str = "Invalid enumeration";
			break;
		case GL_INVALID_VALUE:
			error_str = "Invalid value";
			break;
		case GL_INVALID_OPERATION:
			error_str = "Invalid operation";
			break;
		#ifdef GL_STACK_OVERFLOW
		case GL_STACK_OVERFLOW:
			error_str = "Stack overflow";
			break;
		#endif
		#ifdef GL_STACK_UNDERFLOW
		case GL_STACK_UNDERFLOW:
			error_str = "Stack underflow";
			break;
		#endif
		case GL_OUT_OF_MEMORY:
			error_str = "Out of memory";
			break;
		case GL_NO_ERROR:
			error_str = "No error";
			break;
		default:
			error_str = "Unknown error";
			break;
	}

    if( error != GL_NO_ERROR )
	{
        fprintf( stderr, "[GL] Error %x: %s.\n", error, error_str );
	}

	return error;
}

#if 0
vec4_t viewport_coord_unproject( const vec2_t* position, const mat4_t* projection )
{
	GLint viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT, viewport );

	/* Convert to normalized device coordinates */
	vec4_t normalized_device_coordinate = VEC4_LITERAL( ((position->x * 2.0f) / viewport[2]) - 1.0f, ((position->y * 2.0f) / viewport[3]) - 1.0f, 0.0f, 1.0f );

	mat4_t proj = *projection;
	mat4_invert( &proj );

	return mat4_mult_vector( &proj, &normalized_device_coordinate );
}
#endif

vec4_t viewport_coord_unproject( const vec2_t* position, const mat4_t* projection, const mat4_t* model )
{
	GLint viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT, viewport );

	/* Convert to normalized device coordinates */
	vec4_t normalized_device_coordinate = VEC4_LITERAL( ((position->x * 2.0f) / viewport[2]) - 1.0f, ((position->y * 2.0f) / viewport[3]) - 1.0f, 0.0f, 1.0f );

	mat4_t inv_projmodel = mat4_mult_matrix( projection, model );
	mat4_invert( &inv_projmodel );

	return mat4_mult_vector( &inv_projmodel, &normalized_device_coordinate );
}

vec2_t viewport_coord_project( const vec4_t* point, const mat4_t* projection, const mat4_t* model )
{
	GLint viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT, viewport );

	mat4_t projmodel = mat4_mult_matrix( projection, model );
	vec4_t pt = mat4_mult_vector( &projmodel, point );

	return VEC2_LITERAL( ((1.0f + pt.x) * viewport[2]) / 2.0f, ((1.0f + pt.y) * viewport[3]) / 2.0f );
}
