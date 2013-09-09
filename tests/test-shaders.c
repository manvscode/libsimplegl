#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define ESC_KEY			27
static unsigned short windowWidth  = 0;
static unsigned short windowHeight = 0;

static void initialize( void );
static void deinitialize( void );

static void render( void );
static void resize( int width, int height );
static void keyboard_keypress( unsigned char key, int x, int y );

int main( int argc, char* argv[] )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );

	initialize( );
	glutInitWindowSize( 800, 600 );
	int window = glutCreateWindow( "Test Shader" );

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
	printf( "initialize\n" );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
}

void deinitialize( void )
{
	printf( "deinitialize\n" );
}

void render( void )
{
	glutSwapBuffers( );
}

void resize( int width, int height )
{
	glViewport( 0, 0, width, height );


	#define max( x, y )              ((x) ^ (((x) ^ (y)) & -((x) < (y))))
	height = max( 1, height );

	glOrtho( 0.0, width, 0.0, height, 0.0, 1.0 );

	windowWidth  = width;
	windowHeight = height;
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
