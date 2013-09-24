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

#ifdef SIMPLEGL_DEBUG
static __inline void glsl_shader_error  ( GLuint shader );
static __inline void glsl_program_error ( GLuint program );
#endif


GLboolean glsl_program_from_shaders( GLuint* p_program, const shader_info_t* shaders, GLsizei count, GLchar** shader_log, GLchar** program_log )
{
	const shader_info_t* info;
	GLuint shader_names[ count ]; /* VLAs in C99 */
	GLsizei i;
	GLboolean result = true;

	assert( p_program && shaders );

	for( i = 0; result && i < count; i++ )
	{
		info = &shaders[ i ];

		const char* shader_source_code = glsl_shader_load( info->filename );

		result = glsl_shader_create_from_source( &shader_names[ i ], info->type, shader_source_code, shader_log );
		free( (char*) shader_source_code );
	}

	result = result && glsl_program_create( p_program, shader_names, count, GL_TRUE /* delete shaders when program is deleted */, program_log );

	return result;
}

/*
 * Create a program by attaching and linking a collection of shaders.
 */
GLboolean glsl_program_create( GLuint* p_program, GLuint *shaders, GLsizei shader_count, GLboolean mark_shaders_for_deletion, GLchar** program_log )
{
	assert( p_program );
	GLuint p = glCreateProgram( );
	GLsizei i;

	if( p <= 0 )
	{
		#ifdef SIMPLEGL_DEBUG
		fprintf( stderr, "[GLSL] Failed to create program.\n" );
		#endif
		goto failure;
	}


	for( i = 0; i < shader_count; i++ )
	{
		glsl_attach_shader( p, shaders[ i ] );
	}

	if( !glsl_link_program( p ) )
	{
		/* linker error */
		#ifdef SIMPLEGL_DEBUG
		fprintf( stderr, "[GLSL] Failed to link program.\n" );
		glsl_program_error( p );
		#endif
		goto failure;
	}


	if( mark_shaders_for_deletion )
	{
		/*
		 *  Mark all of the attached shaders to be
		 *  deleted when the program gets deleted
		 *  with glDeleteProgram().
		 */
		for( i = 0; i < shader_count; i++ )
		{
			glsl_destroy( shaders[ i ] );
		}
	}

	*p_program = p;

	if( program_log )
	{
		*program_log = NULL;
	}

	return GL_TRUE;

failure:
	if( program_log )
	{
		*program_log = glsl_log( *p_program );
	}

	if( p )
	{
		glsl_destroy( p );
	}

	return GL_FALSE;
}

/*  Creates, compiles and links shader program.
 *
 * Parameters:
 * shader -- shader name
 *   type -- GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
 * source -- shader source code
 */
GLboolean glsl_shader_create_from_source( GLuint* p_shader, GLenum type, const GLchar* source, GLchar** shader_log )
{
	assert( p_shader );
	GLuint s = glCreateShader( type );
	GL_ASSERT_NO_ERROR( );

	if( !s )
	{
		#ifdef SIMPLEGL_DEBUG
		const char* type_str;
		switch( type )
		{
			#ifdef GL_TESS_CONTROL_SHADER
			case GL_TESS_CONTROL_SHADER:
				type_str = "tesselation control";
				break;
			#endif
			#ifdef GL_TESS_EVALUATION_SHADER
			case GL_TESS_EVALUATION_SHADER:
				type_str = "tesselation evaluation";
				break;
			#endif
			case GL_GEOMETRY_SHADER:
				type_str = "geometry";
				break;
			case GL_FRAGMENT_SHADER:
				type_str = "fragment";
				break;
			case GL_VERTEX_SHADER:
			default:
				type_str = "vertex";
				break;
		}
		fprintf( stderr, "[GLSL] Failed to create %s shader.\n", type_str );
		#endif
		goto failure;
	}


	/* Load shader source */
	#if 0
	const GLchar* sources[ 2 ] = {
	#ifdef GL_ES_VERSION_2_0
		"#version 100\n"
			"#define GLES2\n",
	#else
		"#version 130\n",
	#endif
		source
	};
	/* Hard coding the version in the sources doesn't look like a good idea */
	glShaderSource( s, 2, sources, NULL );
	#else
	glShaderSource( s, 1, &source, NULL );
	#endif
	GL_ASSERT_NO_ERROR( );

	glCompileShader( s );
	GL_ASSERT_NO_ERROR( );

	GLint compileStatus;
	glGetShaderiv( s, GL_COMPILE_STATUS, &compileStatus );
	GL_ASSERT_NO_ERROR( );

	if( compileStatus == GL_FALSE )
	{
		/* Compilation error */
		#ifdef SIMPLEGL_DEBUG
		fprintf( stderr, "[GLSL] Failed to compile shader %u.\n", s );
		glsl_shader_error( s );
		#endif
		goto failure;
	}

	if( shader_log )
	{
		*shader_log = NULL;
	}

	*p_shader = s;

	return GL_TRUE;

failure:
	if( shader_log )
	{
		*shader_log = glsl_log( s );
	}

	if( s )
	{
		glsl_destroy( s );
	}

	return GL_FALSE;
}

GLuint glsl_create( GLenum type )
{
	GLuint object = 0;
	const char* type_str;

	switch( type )
	{
		case GL_VERTEX_SHADER:
			type_str = "vertex shader";
			object = glCreateShader( type );
			break;
		case GL_FRAGMENT_SHADER:
			type_str = "fragment shader";
			object = glCreateShader( type );
			break;
		case GL_GEOMETRY_SHADER:
			type_str = "geometry shader";
			object = glCreateShader( type );
			break;
		#ifdef GL_TESS_EVALUATION_SHADER /* Missing on Mac OS X */
		case GL_TESS_CONTROL_SHADER:
			type_str = "tesselation control shader";
			object = glCreateShader( type );
			break;
		#endif
		#ifdef GL_TESS_EVALUATION_SHADER /* Missing on Mac OS X */
		case GL_TESS_EVALUATION_SHADER:
			type_str = "tesselation evaluation shader";
			object = glCreateShader( type );
			break;
		#endif
		#ifdef GL_PROGRAM /* Missing on Mac OS X */
		case GL_PROGRAM:
		#endif
		default:
			type_str = "program";
			break;
			object = glCreateProgram( );
			break;
	}

	#ifdef SIMPLEGL_DEBUG
	if( !object )
	{
		fprintf( stderr, "[GLSL] Failed to create %s.\n", type_str );
	}
	#endif

	return object;
}

GLboolean glsl_destroy( GLuint object /* program or shader */ )
{
	GLboolean result = GL_FALSE;

	if( glIsProgram( object ) )
	{
		glDeleteProgram( object );
		result = GL_TRUE;
	}
	else if( glIsShader( object ) )
	{
		glDeleteShader( object );
		result = GL_TRUE;
	}

	return result;
}

/*
 * Load a shader program into a string buffer.
 */
GLchar* glsl_shader_load( const char* path )
{
	FILE* file = fopen( path, "r" );
	GLchar* result = NULL;

	if( file )
	{
		fseek( file, 0, SEEK_END );
		long file_size = ftell( file ); /* TODO: what if size is 0 */
		fseek( file, 0, SEEK_SET );

		if( file_size > 0 )
		{
			result = (GLchar*) malloc( sizeof(GLchar) * (file_size + 1) );

			if( result )
			{
				char* buffer = result;
				long size    = file_size;

				while( !feof( file ) && size > 0 )
				{
					size_t bytes_read = fread( buffer, sizeof(GLchar), size, file );
					buffer += bytes_read;
					size   -= bytes_read;
				}
			}

			result[ file_size ] = '\0';
		}

		fclose( file );
	}

#if defined(SIMPLEGL_DEBUG) && 0
	fprintf( stdout, "-------------------- [ bof %s] ---------------------\n", path );
	fprintf( stdout, "%s", result );
	fprintf( stdout, "-------------------- [ eof %s] ---------------------\n", path );
#endif

	return result;
}

GLboolean glsl_shader_compile( GLuint shader, const GLchar* source )
{
	GLboolean result = GL_TRUE;

	if( glIsShader(shader) )
	{
		glShaderSource( shader, 1, &source, NULL );
		glCompileShader( shader );

		GLint compileStatus;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );

		if( !compileStatus )
		{
			/* Compilation error */
			#ifdef SIMPLEGL_DEBUG
			fprintf( stderr, "[GLSL] Failed to compile shader %u.\n", shader );
			glsl_shader_error( shader );
			#endif
		}

		result = (compileStatus == GL_TRUE);
	}

	return result;
}

GLboolean glsl_attach_shader( GLuint program, GLuint shader )
{
	if( glIsProgram(program) && glIsShader(shader) )
	{
		glAttachShader( program, shader );
		return GL_TRUE;
	}

	return GL_FALSE;
}

GLboolean glsl_link_program( GLuint program )
{
	GLboolean result = GL_FALSE;

	if( glIsProgram(program) )
	{
		glLinkProgram( program );

		GLint link_status;
		glGetProgramiv(	program, GL_LINK_STATUS, &link_status );

		if( !link_status )
		{
			/* linker error */
			#ifdef SIMPLEGL_DEBUG
			fprintf( stderr, "[GLSL] Failed to link program.\n" );
			glsl_program_error( program );
			#endif
		}

		result = (link_status == GL_TRUE);
	}

	return result;
}

GLint glsl_bind_attribute( GLuint program, const GLchar* name )
{
	GLuint result = glGetAttribLocation( program, name );
	GL_ASSERT_NO_ERROR( );

	#ifdef SIMPLEGL_DEBUG
	if( result == -1 )
	{
		fprintf( stderr, "[GLSL] Could not bind attribute %s\n", name );
	}
	#endif

	return result;
}

GLint glsl_bind_uniform( GLuint program, const GLchar* name )
{
	GLuint result = glGetUniformLocation( program, name );
	GL_ASSERT_NO_ERROR( );

	#ifdef SIMPLEGL_DEBUG
	if( result == -1 )
	{
		fprintf( stderr, "[GLSL] Could not bind uniform %s\n", name );
	}
	#endif

	return result;
}


GLchar* glsl_log( GLuint object )
{
	GLint log_length = 0;
	GLchar* p_log    = NULL;

	if( glIsShader( object ) )
	{
		glGetShaderiv( object, GL_INFO_LOG_LENGTH, &log_length );
	}
	else if( glIsProgram( object ) )
	{
		glGetProgramiv( object, GL_INFO_LOG_LENGTH, &log_length );
	}

	if( log_length > 0 )
	{
		p_log = malloc( sizeof(GLchar) * log_length );

		memset( p_log, 0, sizeof(GLchar) * log_length );

		if( p_log )
		{
			if( glIsShader( object ) )
			{
				glGetShaderInfoLog( object, log_length, NULL, p_log );
			}
			else if( glIsProgram( object ) )
			{
				glGetProgramInfoLog( object, log_length, NULL, p_log );
			}
		}

		p_log[ log_length - 1 ] ='\0';
	}

	return p_log;
}

#ifdef SIMPLEGL_DEBUG
void glsl_shader_error( GLuint shader )
{
	const GLchar* p_log = glsl_log( shader );

	if( p_log )
	{
		fprintf( stderr, "[Shader %u Error] %s\n", shader, p_log );
		free( (void*) p_log );
	}
	else
	{
		fprintf( stderr, "[Shader %u Error] unknown\n", shader );
	}
}

void glsl_program_error( GLuint program )
{
	const GLchar* p_log = glsl_log( program );

	if( p_log )
	{
		fprintf( stderr, "[Program %u Error] %s\n", program, p_log );
		free( (void*) p_log );
	}
	else
	{
		fprintf( stderr, "[Program %u Error] unknown\n", program );
	}
}
#endif
