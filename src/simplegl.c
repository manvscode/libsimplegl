#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "simplegl.h"

void dump_gl_info( void )
{
	const char* renderer     = (const char*) glGetString(GL_RENDERER);
	const char* version      = (const char*) glGetString(GL_VERSION);
	const char* glsl_version = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);

	fprintf( stdout, "[GL] Renderer: %s\n", renderer ? renderer : "unknown" );
	fprintf( stdout, "[GL] Version: %s\n", version ? version : "unknown" );
	fprintf( stdout, "[GL] Shading Language: %s\n", glsl_version ? glsl_version : "unknown" );
}

void check_gl( void )
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
		default:
			error_str = "No error";
			break;
	}

    if( error != GL_NO_ERROR )
	{
        fprintf( stderr, "[GL Error %x] %s\n", error, error_str );
	}

	#ifdef SIMPLEGL_DEBUG
	/* Has an error occurred? */
	assert( GL_NO_ERROR == error );
	#endif
}

