#include <GL/gl.h>
#include <GL/glext.h>
#include "simplegl.h"

static __inline void simplegl_shader_error  ( GLuint shader );
static __inline void simplegl_program_error ( GLuint program );


/*  Creates, compiles and links shader program.
 *
 * Parameters:
 * shader -- shader name
 *   type -- GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
 * source -- shader source code
 */
GLboolean simplegl_shader_create_from_source( GLuint* shader, GLenum type, const GLchar* source )
{
    GLuint s = glCreateShader( type );

    if( !s )
    {
        return GL_FALSE;
    }

    /* Load shader source */
    glShaderSource( s, 1, source, NULL );

    glCompileShader( s );

    GLint compileStatus;
    glGetShader( s, GL_COMPILE_STATUS, &compileStatus );

    if( compileStatus == GL_FALSE )
    {
        /* Compilation error */
        #ifdef SIMPLEGL_DEBUG
        simplegl_shader_error( s );
        #endif
        return GL_FALSE;
    }

    *shader = s;
    return GL_TRUE;
}


const GLchar* simplegl_shader_log( GLuint shader )
{
    GLint log_length;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &log_length );

    GLchar* log = malloc( sizeof(GLchar) * log_length );

    if( log )
    {
        glGetShaderInfoLog( shader, log_length, NULL, log );
    }

    return log;
}

void simplegl_shader_error( GLuint shader )
{
    const GLchar* log = simplegl_shader_log( shader );

    if( log )
    {
        fprintf( stderr, "[Shader %zu Error] %s\n", shader, log );
        free( log );
    }
    else
    {
        fprintf( stderr, "[Shader %zu Error] unknown\n", shader );
    }
}

GLboolean simplegl_program_create( GLuint* program, GLuint *shaders, GLsizei shader_count )
{
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
        simplegl_program_error( p )
        #endif
        return GL_FALSE;
    }

    *program = p;
    return GL_TRUE;
}

const GLchar* simplegl_program_log( GLuint program )
{
    GLint log_length;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &log_length );

    GLchar* log = malloc( sizeof(GLchar) * log_length );

    if( log )
    {
        glGetProgramInfoLog( program, log_length, NULL, log );
    }

    return log;
}

void simplegl_program_error( GLuint program )
{
    const GLchar* log = simplegl_program_log( program );

    if( log )
    {
        fprintf( stderr, "[Program %zu Error] %s\n", program, log );
        free( log );
    }
    else
    {
        fprintf( stderr, "[Program %zu Error] unknown\n", program );
    }
}
