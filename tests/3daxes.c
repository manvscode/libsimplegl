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
#include "../src/simplegl.h"
#include <SDL2/SDL.h>

static void initialize     ( void );
static void deinitialize   ( void );
static void render         ( void );
static void dump_sdl_error ( void );

SDL_Window* window = NULL;
SDL_GLContext ctx = NULL;

axes_3d_t axes;

int main( int argc, char* argv[] )
{
	if( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		goto quit;
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4 );

	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	window = SDL_CreateWindow( "3D Axes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags );

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

	initialize( );

	/* event loop */
	{
		SDL_Event e;
		bool done = false;
		bool fullscreen = false;

		while( !done )
		{
			SDL_PollEvent( &e );

			switch( e.type )
			{
				case SDL_QUIT:
				{
					done = true;
					break;
				}
				case SDL_KEYDOWN:
				{
					switch( e.key.keysym.sym )
					{
						case SDLK_ESCAPE:
							done = true;
							break;
						case SDLK_f:
							fullscreen ^= true;
							SDL_SetWindowFullscreen( window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
							break;
					}
					break;
				}
			}

			render( );
			//SDL_Delay(10);              // Pause briefly before moving on to the next cycle.
		}
	}

	deinitialize( );

quit:
	if( ctx ) SDL_GL_DeleteContext( ctx );
	if( window ) SDL_DestroyWindow( window );
	SDL_Quit( );
	return 0;
}

void initialize( void )
{
	gl_info_print( );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	GL_ASSERT_NO_ERROR( );

	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );

	glLineWidth( 1.0f );
	GL_ASSERT_NO_ERROR( );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	GL_ASSERT_NO_ERROR( );

	axes = axes_3d_create( 1.0f );

	glPointSize( 1.0 );


	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	glViewport(0, 0, width, height );
	GL_ASSERT_NO_ERROR( );
}

void deinitialize( void )
{
	axes_3d_destroy( &axes );
}

void render( )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	static float angle = 0.0;
	if( angle >= 360.0f ) angle = 0.0f;

	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	GLfloat aspect = ((GLfloat)height) / width;
	vec3_t translation = VEC3( 0.0, 0.0, -2.0f );
	mat4_t projection = m3d_perspective( 45.0, aspect, 0.00005, 100.0 );
	mat4_t rotation = m3d_rotate_xyz( "xy", -15.0f * M3D_RADIANS_PER_DEGREE, angle );
	angle += 0.005;
	mat4_t transform = m3d_translate( &translation );
	transform = mat4_mult_matrix( &transform, &rotation );
	mat4_t model_view = mat4_mult_matrix( &projection, &transform );

	axes_3d_render( axes, model_view.m );

	SDL_GL_SwapWindow( window );
}

void dump_sdl_error( void )
{
	const char* sdl_error = SDL_GetError( );

	if( sdl_error && *sdl_error != '\0' )
	{
		fprintf( stderr, "[SDL] %s\n", sdl_error );
	}
}
