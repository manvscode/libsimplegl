#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "../src/simplegl.h"

#define ESC_KEY			27

static void initialize( void );
static void deinitialize( void );

static void render( void );
static void resize( int width, int height );
static void keyboard_keypress( unsigned char key, int x, int y );

GLuint program;
GLint attribute_vertex;
GLint attribute_color;

int main( int argc, char* argv[] )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );

	glutInitWindowSize( 800, 800 );
	int window = glutCreateWindow( "Test Shader" );

	#if 0
	GLenum err = glewInit( );
	if( GLEW_OK != err )
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	#endif

	initialize( );

	glutDisplayFunc( render );
	glutReshapeFunc( resize );
	glutKeyboardFunc( keyboard_keypress );
	//glutIdleFunc( idle );
	//glutMouseFunc( mouse );
	//glutMotionFunc( mouse_motion );


	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

	glutMainLoop( );
	deinitialize( );
	return 0;
}

void initialize( void )
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );


	//glEnable( GL_BLEND );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	GLchar* shader_log  = NULL;
	GLchar* program_log = NULL;
	const shader_info_t shaders[] = {
		{ GL_VERTEX_SHADER,   "./tests/test-shaders.vert" },
		{ GL_FRAGMENT_SHADER, "./tests/test-shaders.frag" }
	};

	if( !glsl_program_from_shaders( &program, shaders, shader_info_count(shaders), &shader_log, &program_log ) )
	{
		if( shader_log )
		{
			printf( " [Shader Log] %s\n", shader_log );
			free( shader_log );
		}
		if( program_log )
		{
			printf( "[Program Log] %s\n", program_log );
			free( program_log );
		}

		glutLeaveMainLoop( );
		return;
	}

	attribute_vertex = glsl_bind_attribute( program, "vertex" );
	attribute_color  = glsl_bind_attribute( program, "color" );

	GLdouble vertices[] = {
		 0.8,  0.8,
		-0.8, -0.8,
		 0.8, -0.8,
	};

	GLfloat colors[] = {
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0
	};

	glVertexAttribPointer( attribute_vertex, 2, GL_DOUBLE, GL_FALSE, 0, vertices );
	glVertexAttribPointer( attribute_color, 3, GL_FLOAT, GL_FALSE, 0, colors );
}

void deinitialize( void )
{
	glDeleteProgram( program );
}

void render( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glEnableVertexAttribArray( attribute_vertex );
	glEnableVertexAttribArray( attribute_color );
	glUseProgram( program );
	glDisableVertexAttribArray( attribute_vertex );
	glDisableVertexAttribArray( attribute_color );

	glDrawArrays( GL_TRIANGLES, 0, 3 );

	glutSwapBuffers( );
	GL_ASSERT_NO_ERROR();
}

void resize( int width, int height )
{
	glViewport( 0, 0, width, height );


	#define max( x, y )              ((x) ^ (((x) ^ (y)) & -((x) < (y))))
	height = max( 1, height );

	//glOrtho( 0.0, width, 0.0, height, 0.0, 1.0 );
}

void keyboard_keypress( unsigned char key, int x, int y )
{
	switch( key )
	{
		case ESC_KEY:
			glutLeaveMainLoop( );
			break;
		default:
			break;
	}

}
