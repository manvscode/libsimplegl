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
#include <lib3dmath/vec3.h>
#include <lib3dmath/vec4.h>
#include <lib3dmath/quat.h>
#include <lib3dmath/geometric-tools.h>
#include <SDL2/SDL.h>

static void initialize     ( void );
static void deinitialize   ( void );
static void render         ( void );
static void dump_sdl_error ( void );

static SDL_Window* window = NULL;
static SDL_GLContext ctx = NULL;

#define FLAG_WIDTH   154
#define FLAG_HEIGHT  100
#define INDICES_COUNT  6 * (FLAG_WIDTH - 1) * (FLAG_HEIGHT - 1)

static GLuint program              = 0;
static GLint uniform_model_view    = 0;
static GLint uniform_normal_matrix = 0;
static GLint uniform_time          = 0;
static GLint uniform_texture       = 0;

static GLuint vao          = 0;
static GLuint vbo_vertices = 0;
static GLuint vbo_normals  = 0;
static GLuint vbo_indices  = 0;
static GLuint flag_texture = 0;
static GLuint subtitle_texture_overlay = 0;
static raster_font_t* font1 = NULL;
static raster_font_t* font2 = NULL;
static quat_t flag_orientation = QUAT(0,1,0,0);
static vec3_t vertices[ FLAG_WIDTH * FLAG_HEIGHT ];
static vec3_t normals[ FLAG_WIDTH * FLAG_HEIGHT ];
static GLushort indices[ INDICES_COUNT ];


int main( int argc, char* argv[] )
{
	if( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		goto quit;
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8 );

	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	window = SDL_CreateWindow( "Flag Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags );

	if( window == NULL )
	{
		dump_sdl_error( );
		goto quit;
	}

	ctx = SDL_GL_CreateContext( window );
	SDL_GL_SetSwapInterval( 1 ); /* vsync */

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

		SDL_SetWindowFullscreen( window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
		SDL_ShowCursor( fullscreen ? SDL_DISABLE : SDL_ENABLE );

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
						case SDLK_q:
							done = true;
							break;
						case SDLK_f:
							fullscreen ^= true;
							SDL_SetWindowFullscreen( window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
							SDL_ShowCursor( fullscreen ? SDL_DISABLE : SDL_ENABLE );
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
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	GL_ASSERT_NO_ERROR( );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	font1 = raster_font_create( RASTER_FONT_FONT1_8X8 );
	if( !font1 )
	{
		printf( "Unable to create raster font.\n" );
		exit( EXIT_FAILURE );
	}

	font2 = raster_font_create( RASTER_FONT_FONT3_16X16 );
	if( !font2 )
	{
		printf( "Unable to create raster font.\n" );
		exit( EXIT_FAILURE );
	}

	GLchar* shader_log  = NULL;
	GLchar* program_log = NULL;
	const shader_info_t shaders[] = {
		{ GL_VERTEX_SHADER,   "assets/shaders/flag-150.v.glsl" },
		{ GL_FRAGMENT_SHADER, "assets/shaders/flag-150.f.glsl" }
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

		exit( EXIT_FAILURE );
	}

	uniform_model_view = glsl_bind_uniform( program, "u_model_view" );
	GL_ASSERT_NO_ERROR( );
	uniform_normal_matrix = glsl_bind_uniform( program, "u_normal_matrix" );
	GL_ASSERT_NO_ERROR( );
	uniform_time       = glsl_bind_uniform( program, "u_time" );
	GL_ASSERT_NO_ERROR( );
	uniform_texture = glsl_bind_uniform( program, "u_texture" );
	GL_ASSERT_NO_ERROR( );

	flag_texture = tex_create( );
	GL_ASSERT_NO_ERROR( );
	if( flag_texture )
	{
		glActiveTexture( GL_TEXTURE0 );
		GL_ASSERT_NO_ERROR( );
		tex_load_2d_with_linear( flag_texture, "assets/textures/us-flag.png", TEX_CLAMP_S | TEX_CLAMP_T );
		GL_ASSERT_NO_ERROR( );
	}
	else
	{
		dump_sdl_error( );
	}

	subtitle_texture_overlay = tex_create( );

	if( subtitle_texture_overlay )
	{
		assert( gl_error() == GL_NO_ERROR );
		if( !tex_load_2d( subtitle_texture_overlay, "assets/textures/subtitle-overlay.png", GL_NEAREST, GL_NEAREST, TEX_CLAMP_S | TEX_CLAMP_T ) )
		{
			dump_sdl_error( );
			exit( EXIT_FAILURE );
		}

		assert( gl_error() == GL_NO_ERROR );
	}
	else
	{
		dump_sdl_error( );
	}


	for( int j = 0; j < FLAG_HEIGHT; j++ )
	{
		for( int i = 0; i < FLAG_WIDTH; i++ )
		{
			vertices[ i +  FLAG_WIDTH * j ] = VEC3( i - FLAG_WIDTH / 2.0f, j - FLAG_HEIGHT / 2.0f, 0.0 );
		}
	}

	size_t count = 0;
	for( int j = 0; j < FLAG_HEIGHT - 1; j++ )
	{
		for( int i = 0; i < FLAG_WIDTH - 1; i++ )
		{
			indices[ count++ ] = (i + 0) +  FLAG_WIDTH * (j + 1);
			indices[ count++ ] = (i + 0) +  FLAG_WIDTH * (j + 0);
			indices[ count++ ] = (i + 1) +  FLAG_WIDTH * (j + 0);

			indices[ count++ ] = (i + 0) +  FLAG_WIDTH * (j + 1);
			indices[ count++ ] = (i + 1) +  FLAG_WIDTH * (j + 0);
			indices[ count++ ] = (i + 1) +  FLAG_WIDTH * (j + 1);
		}
	}

	for( int j = 0; j < FLAG_HEIGHT - 1; j++ )
	{
		for( int i = 0; i < FLAG_WIDTH - 1; i++ )
		{

			if( j >= 1 && i >= 1 && j < (FLAG_HEIGHT - 1) && i < (FLAG_WIDTH - 1) )
			{
				/*
				 *                             1--2--*
				 *  The numbers are the        |\ |\ |
				 *  vertices that are members  | \| \|
				 *  of the 6 triangles shared  6--0--3
				 *  by vertex 0.               |\ |\ |
				 *                             | \| \|
				 *                             *--5--4
				 */
				indices[ count++ ] = (i + 0) +  FLAG_WIDTH * (j + 0);
				indices[ count++ ] = (i + 1) +  FLAG_WIDTH * (j + 0);

				indices[ count++ ] = (i + 0) +  FLAG_WIDTH * (j + 1);
				indices[ count++ ] = (i + 1) +  FLAG_WIDTH * (j + 0);
				indices[ count++ ] = (i + 1) +  FLAG_WIDTH * (j + 1);

				const vec3_t* points[ 6 ] = {
					&vertices[ (i - 1) +  FLAG_WIDTH * (j - 1) ],
					&vertices[ (i + 0) +  FLAG_WIDTH * (j - 1) ],
					&vertices[ (i + 1) +  FLAG_WIDTH * (j + 0) ],
					&vertices[ (i + 1) +  FLAG_WIDTH * (j + 1) ],
					&vertices[ (i + 0) +  FLAG_WIDTH * (j + 1) ],
					&vertices[ (i - 1) +  FLAG_WIDTH * (j + 0) ],
				};

				normals[ i + FLAG_WIDTH * j ] = normal_from_triangles( points, 6 );
			}
			else
			{
				normals[ i + FLAG_WIDTH * j ] = VEC3_ZUNIT;
			}
		}
	}

	if( buffer_create( &vbo_vertices, vertices, sizeof(GLfloat), FLAG_WIDTH * FLAG_HEIGHT * sizeof(vec3_t), GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
	}
	else
	{
		dump_sdl_error( );
		exit( EXIT_FAILURE );
	}

	if( buffer_create( &vbo_normals, normals, sizeof(GLfloat), FLAG_WIDTH * FLAG_HEIGHT * sizeof(vec3_t), GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
	}
	else
	{
		dump_sdl_error( );
		exit( EXIT_FAILURE );
	}

	if( buffer_create( &vbo_indices, indices, sizeof(GLushort), INDICES_COUNT, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
	}
	else
	{
		dump_sdl_error( );
		exit( EXIT_FAILURE );
	}

	glGenVertexArrays( 1, &vao );
	GL_ASSERT_NO_ERROR( );
	glBindVertexArray( vao );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	GL_ASSERT_NO_ERROR( );

	glBindBuffer( GL_ARRAY_BUFFER, vbo_vertices );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, vbo_normals );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo_indices );

	glPointSize( 1.0 );
	glLineWidth( 1.0 );

	overlay_initialize( );

	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	glViewport(0, 0, width, height );
	GL_ASSERT_NO_ERROR( );
}

void deinitialize( void )
{
	tex_destroy( flag_texture );
	tex_destroy( subtitle_texture_overlay );
	glDeleteVertexArrays( 1, &vao );
	glDeleteBuffers( 1, &vbo_vertices );
	glDeleteBuffers( 1, &vbo_normals );
	glDeleteBuffers( 1, &vbo_indices );
	glDeleteProgram( program );
	raster_font_destroy( font1 );
	raster_font_destroy( font2 );
	overlay_deinitialize( );
}

GLuint delta = 0;

void render( )
{
	GLuint now = SDL_GetTicks( );
	delta = frame_delta( now /* milliseconds */ );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	GLfloat aspect = ((GLfloat)width) / height;
	vec3_t translation = VEC3( 0, 0, -160 );
	#if 1
	mat4_t projection = perspective( 45.0 * RADIANS_PER_DEGREE, aspect, 5, 500.0 );
	#else
	mat4_t projection = orthographic( -100.0f, 100.0f, -100.0f, 100.0f, -300.0f, 300.0f );
	#endif
	const float angle = 0.01f;
	quat_t q = quat_from_axis3_angle( &VEC3(0, 1, 0), angle * RADIANS_PER_DEGREE * delta );
	flag_orientation = quat_multiply( &flag_orientation, &q );
	mat4_t rotation = quat_to_mat4( &flag_orientation );
	mat4_t transform = translate( &translation );
	transform = mat4_mult_matrix( &transform, &rotation );

	mat3_t normal_matrix = MAT3(
		transform.m[0], transform.m[1], transform.m[ 2],
		transform.m[4], transform.m[5], transform.m[ 6],
		transform.m[8], transform.m[9], transform.m[10]
	);
	mat3_transpose( &normal_matrix );

	mat4_t model_view = mat4_mult_matrix( &projection, &transform );


	glUseProgram( program );
	glBindVertexArray( vao );
	assert(gl_error() == GL_NO_ERROR);
	glUniformMatrix4fv( uniform_model_view, 1, GL_FALSE, model_view.m );
	glUniformMatrix3fv( uniform_normal_matrix, 1, GL_FALSE, normal_matrix.m );
	glUniform1i( uniform_time, now );
	assert(gl_error() == GL_NO_ERROR);

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, flag_texture );
	glUniform1i( uniform_texture, 0 );
	assert(gl_error() == GL_NO_ERROR);
	//glDrawArrays( GL_POINTS, 0, FLAG_WIDTH * FLAG_HEIGHT );
	glDrawElements( GL_TRIANGLES, INDICES_COUNT, GL_UNSIGNED_SHORT, 0 );
	assert(gl_error() == GL_NO_ERROR);

	glBindVertexArray( 0 );
	GL_ASSERT_NO_ERROR( );

	overlay_render( &VEC2(0,0), &VEC2(540,32), &VEC3(1,1,1), subtitle_texture_overlay );

	raster_font_shadowed_writef( font2, &VEC2(2, 2 + 8 * 1.5f ), &VEC3(1,1,0), &VEC3_ZERO, 0.8f, "Flag Waving Demo" );
	raster_font_shadowed_writef( font1, &VEC2(2, 2), &VEC3(1,1,1), &VEC3_ZERO, 1.0f, "FPS: %.1f", frame_rate(delta) );

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
