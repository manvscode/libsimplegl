/* Copyright (C) 2013-2014 by Joseph A. Marrero, http://www.manvscode.com/
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

#define UNKNOWN   "unknown"

const GLchar* gl_vendor( void )
{
	const GLchar* vendor = (const GLchar*) glGetString(GL_VENDOR);
	return vendor ? vendor : UNKNOWN;
}

const GLchar* gl_renderer( void )
{
	const GLchar* renderer = (const GLchar*) glGetString(GL_RENDERER);
	return renderer ? renderer : UNKNOWN;
}

const GLchar* gl_version( void )
{
	const GLchar* version = (const GLchar*) glGetString(GL_VERSION);
	return version ? version : UNKNOWN;
}

const GLchar* gl_shader_version( void )
{
	const GLchar* glsl_version = (const GLchar*) glGetString(GL_SHADING_LANGUAGE_VERSION);
	return glsl_version ? glsl_version : UNKNOWN;
}

void gl_info_print( void )
{
	const GLchar* vendor       = gl_vendor();
	const GLchar* renderer     = gl_renderer();
	const GLchar* version      = gl_version();
	const GLchar* glsl_version = gl_shader_version();

	fprintf( stdout, "[GL] Vendor: %s\n", vendor );
	fprintf( stdout, "[GL] Renderer: %s\n", renderer );
	fprintf( stdout, "[GL] Version: %s\n", version );
	fprintf( stdout, "[GL] Shading Language: %s\n", glsl_version );

	#ifdef GL_NUM_EXTENSIONS
	fprintf( stdout, "[GL] Extensions: " );

	int extensionsCount = 0;
	assert(gl_error() == GL_NO_ERROR);
	glGetIntegerv( GL_NUM_EXTENSIONS, &extensionsCount );
	assert(gl_error() == GL_NO_ERROR);

	for( int i = 0; i < extensionsCount; i++ )
	{
		printf( "%s%s", glGetStringi( GL_EXTENSIONS, i ), i < (extensionsCount - 1) ? ", " : "\n" );
	}
	#endif
}

GLenum gl_error( void )
{
	GLenum error = glGetError();
	const GLchar* error_str;

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

GLuint frame_delta( GLuint now /* milliseconds */ )
{
	static GLuint last_render = 0;
	GLuint delta = now - last_render;
	last_render = now;
	return delta;
}

GLfloat frame_rate( GLuint delta /* milliseconds */ )
{
	static GLfloat last_time1 = 0;
	static GLfloat last_time2 = 0;
	#if 1
	delta = (0.6f * delta + 0.3f * last_time1 + 0.1f * last_time2);
	#else
	delta = (0.65f * delta + 0.35f * last_time1 + 0.1f * last_time2);
	#endif
	last_time2 = last_time1;
	last_time1 = delta;
	return (1000.0f) / delta;
}

void frame_rate_print( GLuint delta /* milliseconds */ )
{
	static GLuint frame_rate_call_count = 0;

	if( frame_rate_call_count++ > 60 )
	{
		frame_rate_call_count = 0;
		printf( "fps %.2f\n", frame_rate( delta ) );
	}
}

