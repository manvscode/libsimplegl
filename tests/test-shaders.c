#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

static void render( void );
static void gl_info( void );
static void dump_sdl_error( void );

SDL_Window* window = NULL;
SDL_GLContext ctx = NULL;

GLuint program = 0;
GLint attribute_vertex = 0;
GLint attribute_color = 0;


int main( int argc, char* argv[] )
{
	if( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		goto quit;
	}

	//SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	//SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	//SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	window = SDL_CreateWindow( "Test Shaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

	if( window == NULL )
	{
		dump_sdl_error( );
		goto quit;
	}

	ctx = SDL_GL_CreateContext( window );

	if( !ctx )
	{
		dump_sdl_error( );
		goto quit;
	}

	dump_sdl_error( );

	gl_info( );


	render( );

quit:
	if( ctx ) SDL_GL_DeleteContext( ctx );
	if( window ) SDL_DestroyWindow( window );
	SDL_Quit( );
	return 0;
}


void render( )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	glClearColor( 1.0, 0.0, 0.0, 1.0 );

	SDL_GL_SwapWindow( window );
}

void gl_info( void )
{
	const char* renderer     = (const char*) glGetString(GL_RENDERER);
	const char* version      = (const char*) glGetString(GL_VERSION);
	const char* glsl_version = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);

	fprintf( stdout, "[GL] Renderer: %s\n", renderer ? renderer : "unknown" );
	fprintf( stdout, "[GL] Version: %s\n", version ? version : "unknown" );
	fprintf( stdout, "[GL] Shading Language: %s\n", glsl_version ? glsl_version : "unknown" );
}


void dump_sdl_error( void )
{
	const char* sdl_error = SDL_GetError( );

	if( sdl_error && *sdl_error != '\0' )
	{
		fprintf( stderr, "[SDL] %s\n", sdl_error );
	}
}
