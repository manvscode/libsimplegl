#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "simplegl.h"

static __inline void simplegl_shader_error  ( GLuint shader );
static __inline void simplegl_program_error ( GLuint program );
static char* shader_load( const char* path );

GLboolean simplegl_program_from_shaders( GLuint* p_program, const shader_info_t* shaders, GLsizei count )
{
	const shader_info_t* info;
	GLuint shader_names[ count ]; /* VLAs in C99 */
    GLsizei i;
	GLboolean result = true;

	assert( p_program && shaders );

    for( i = 0; result && i < count; i++ )
    {
		info = &shaders[ i ];

		const char* shader_source_code = shader_load( info->filename );

		result = simplegl_shader_create_from_source( &shader_names[ i ], info->type, shader_source_code );
		free( (char*) shader_source_code );
    }

	result = result && simplegl_program_create( p_program, shader_names, count, GL_TRUE /* delete shaders when program is deleted */ );

	return result;
}

/*  Creates, compiles and links shader program.
 *
 * Parameters:
 * shader -- shader name
 *   type -- GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
 * source -- shader source code
 */
GLboolean simplegl_shader_create_from_source( GLuint* p_shader, GLenum type, const GLchar* source )
{
	assert( p_shader );
    GLuint s = glCreateShader( type );

    if( !s )
    {
        return GL_FALSE;
    }

    /* Load shader source */
    glShaderSource( s, 1, &source, NULL );

    glCompileShader( s );

    GLint compileStatus;
    glGetShaderiv( s, GL_COMPILE_STATUS, &compileStatus );

    if( compileStatus == GL_FALSE )
    {
        /* Compilation error */
        #ifdef SIMPLEGL_DEBUG
        simplegl_shader_error( s );
        #endif
        return GL_FALSE;
    }

    *p_shader = s;
    return GL_TRUE;
}


const GLchar* simplegl_shader_log( GLuint shader )
{
    GLint log_length;
	GLchar* p_log = NULL;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &log_length );

	if( log_length > 0 )
	{
		p_log = malloc( sizeof(GLchar) * log_length );

		if( p_log )
		{
			glGetShaderInfoLog( shader, log_length, NULL, p_log );
		}
	}

    return p_log;
}

void simplegl_shader_error( GLuint shader )
{
    const GLchar* p_log = simplegl_shader_log( shader );

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

GLboolean simplegl_program_create( GLuint* p_program, GLuint *shaders, GLsizei shader_count, GLboolean mark_shaders_for_deletion )
{
	assert( p_program );
    GLuint p = glCreateProgram( );
    GLsizei i;

    for( i = 0; i < shader_count; i++ )
    {
        glAttachShader( p, shaders[ i ] );
    }

    glLinkProgram( p );

    GLint link_status;
    glGetProgramiv(	p, GL_LINK_STATUS, &link_status );

    if( link_status == GL_FALSE )
    {
        /* linker error */
        #ifdef SIMPLEGL_DEBUG
        simplegl_program_error( p );
        #endif
        return GL_FALSE;
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
			glDeleteShader( shaders[ i ] );
		}
	}

    *p_program = p;
    return GL_TRUE;
}

const GLchar* simplegl_program_log( GLuint program )
{
    GLint log_length;
    GLchar* p_log = NULL;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &log_length );

	if( log_length > 0 )
	{
		p_log = malloc( sizeof(GLchar) * log_length );

		if( p_log )
		{
			glGetProgramInfoLog( program, log_length, NULL, p_log );
		}
	}

    return p_log;
}

void simplegl_program_error( GLuint program )
{
    const GLchar* p_log = simplegl_program_log( program );

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

char* shader_load( const char* path )
{
	FILE* file = fopen( path, "r" );
	char* result = NULL;

	if( file )
	{
		fseek( file, 0, SEEK_END );
		size_t file_size = ftell( file ); /* TODO: what if size is 0 */
		fseek( file, 0, SEEK_SET );

		if( file_size > 0 )
		{
			result = (char*) malloc( sizeof(char) * (file_size + 1) );

			if( result )
			{
				char* buffer = result;
				while( !feof( file ) )
				{
					size_t bytes_read = fread( buffer, sizeof(char), file_size, file );
					buffer += bytes_read;
				}
				buffer[ file_size + 1 ] = '\0';
			}
		}

		fclose( file );
	}

	return result;
}
