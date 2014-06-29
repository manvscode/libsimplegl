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
#include <SDL2/SDL.h>
#include <lib3dmath/vec3.h>
#include <lib3dmath/quat.h>
#include "../src/simplegl.h"

static void initialize        ( void );
static void deinitialize      ( void );
static void render            ( void );
static void render_skybox     ( void );
static void dump_sdl_error    ( void );
static void set_view_by_mouse ( void );

SDL_Window* window = NULL;
SDL_GLContext ctx = NULL;

GLuint program = 0;
GLint attribute_vertex = 0;
GLint uniform_eye_position = 0;
GLint uniform_projection = 0;
GLint uniform_orientation = 0;
GLint uniform_cubemap = 0;

GLuint vao = 0;
GLuint vbo_vertices = 0;
GLuint vbo_tex_coords = 0;
GLuint ibo_indices = 0;
GLuint cube_map_texture[ 3 ] = { 0 };
GLuint selected_skybox = 0;
const float mouse_sensitivity = 0.001f;

static const GLfloat cube_map_vertices[] = {
	-10.0f,-10.0f,-10.0f,
	-10.0f,-10.0f, 10.0f,
	-10.0f, 10.0f, 10.0f,
	 10.0f, 10.0f,-10.0f,
	-10.0f,-10.0f,-10.0f,
	-10.0f, 10.0f,-10.0f,
	 10.0f,-10.0f, 10.0f,
	-10.0f,-10.0f,-10.0f,
	 10.0f,-10.0f,-10.0f,
	 10.0f, 10.0f,-10.0f,
	 10.0f,-10.0f,-10.0f,
	-10.0f,-10.0f,-10.0f,
	-10.0f,-10.0f,-10.0f,
	-10.0f, 10.0f, 10.0f,
	-10.0f, 10.0f,-10.0f,
	 10.0f,-10.0f, 10.0f,
	-10.0f,-10.0f, 10.0f,
	-10.0f,-10.0f,-10.0f,
	-10.0f, 10.0f, 10.0f,
	-10.0f,-10.0f, 10.0f,
	 10.0f,-10.0f, 10.0f,
	 10.0f, 10.0f, 10.0f,
	 10.0f,-10.0f,-10.0f,
	 10.0f, 10.0f,-10.0f,
	 10.0f,-10.0f,-10.0f,
	 10.0f, 10.0f, 10.0f,
	 10.0f,-10.0f, 10.0f,
	 10.0f, 10.0f, 10.0f,
	 10.0f, 10.0f,-10.0f,
	-10.0f, 10.0f,-10.0f,
	 10.0f, 10.0f, 10.0f,
	-10.0f, 10.0f,-10.0f,
	-10.0f, 10.0f, 10.0f,
	 10.0f, 10.0f, 10.0f,
	-10.0f, 10.0f, 10.0f,
 	 10.0f,-10.0f, 10.0f
};

GLuint delta = 0;

camera_t* camera = NULL;
raster_font_t* font1 = NULL;
raster_font_t* font2 = NULL;



int main( int argc, char* argv[] )
{
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
	{
		goto quit;
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8 );

	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	window = SDL_CreateWindow( "Sky Box", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, flags );

	if( window == NULL )
	{
		dump_sdl_error( );
		goto quit;
	}

	ctx = SDL_GL_CreateContext( window );
	SDL_GL_SetSwapInterval( 1 ); /* vsync */

	assert(check_gl() == GL_NO_ERROR);
	if( !ctx )
	{
		dump_sdl_error( );
		goto quit;
	}

	initialize( );

	bool done = false;
	bool fullscreen = false;

	SDL_SetWindowFullscreen( window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
	SDL_ShowCursor( fullscreen ? SDL_DISABLE : SDL_ENABLE );

	while( !done )
	{
		SDL_PumpEvents();
		const Uint8 *state = SDL_GetKeyboardState(NULL);

		if( state[SDL_SCANCODE_ESCAPE] || state[SDL_SCANCODE_Q] )
		{
			done = true;
		}
		if( state[SDL_SCANCODE_F] )
		{
			fullscreen ^= true;
			SDL_SetWindowFullscreen( window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
			SDL_ShowCursor( fullscreen ? SDL_DISABLE : SDL_ENABLE );
		}
		if( state[SDL_SCANCODE_W] )
		{
			camera_move_forwards( camera, 2.0f );
		}
		if( state[SDL_SCANCODE_S] )
		{
			camera_move_forwards( camera, -2.0f );
		}
		if( state[SDL_SCANCODE_A] )
		{
			camera_move_sideways( camera, 2.0f );
		}
		if( state[SDL_SCANCODE_D] )
		{
			camera_move_sideways( camera, -2.0f );
		}

		if( state[SDL_SCANCODE_1] )
		{
			selected_skybox = 0;
		}
		else if( state[SDL_SCANCODE_2] )
		{
			selected_skybox = 1;
		}
		else if( state[SDL_SCANCODE_3] )
		{
			selected_skybox = 2;
		}

		render( );
        set_view_by_mouse( );
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
	assert(check_gl() == GL_NO_ERROR);
	dump_gl_info( );
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	assert(check_gl() == GL_NO_ERROR);

	glEnable( GL_DEPTH_TEST );
	//glEnable( GL_CULL_FACE );
	glDisable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	assert(check_gl() == GL_NO_ERROR);

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //glEnable( GL_TEXTURE_CUBE_MAP );
	glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
	assert(check_gl() == GL_NO_ERROR);

	font2 = raster_font_create( RASTER_FONT_VINCENT_8X8 );
	if( !font2 )
	{
		printf( "Unable to create raster font.\n" );
		exit( EXIT_FAILURE );
	}

	font1 = raster_font_create( RASTER_FONT_FONT1_8X8 );
	if( !font1 )
	{
		printf( "Unable to create raster font.\n" );
		exit( EXIT_FAILURE );
	}

	GLchar* shader_log  = NULL;
	GLchar* program_log = NULL;
	const shader_info_t shaders[] = {
		{ GL_VERTEX_SHADER,   "./tests/assets/shaders/skybox.v.glsl" },
		{ GL_FRAGMENT_SHADER, "./tests/assets/shaders/skybox.f.glsl" }
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

	attribute_vertex = glsl_bind_attribute( program, "a_vertex" );
	assert(check_gl() == GL_NO_ERROR);
	uniform_eye_position = glsl_bind_uniform( program, "u_eye_position" );
	assert(check_gl() == GL_NO_ERROR);
	uniform_projection = glsl_bind_uniform( program, "u_projection" );
    assert(check_gl() == GL_NO_ERROR);

    uniform_orientation = glsl_bind_uniform( program, "u_orientation" );
    assert(check_gl() == GL_NO_ERROR);
	uniform_cubemap = glsl_bind_uniform( program, "u_cubemap" );
	assert(check_gl() == GL_NO_ERROR);


	for( int i = 0; i < 3; i++ )
	{
		cube_map_texture[ i ] = tex_create( );
		assert(check_gl() == GL_NO_ERROR);
		if( cube_map_texture[ i ] )
		{
			glActiveTexture( GL_TEXTURE0 );
			assert(check_gl() == GL_NO_ERROR);
			bool cubemap_setup = false;

			if( i == 0 )
			{
				cubemap_setup = tex_cube_map_setup( cube_map_texture[ i ], "./tests/assets/env/gardenrt.png", "./tests/assets/env/gardenlf.png",
																           "./tests/assets/env/gardenup.png", "./tests/assets/env/gardendn.png",
																           "./tests/assets/env/gardenft.png", "./tests/assets/env/gardenbk.png" );
			}
			else if( i == 1 )
			{
				cubemap_setup = tex_cube_map_setup( cube_map_texture[ i ], "./tests/assets/env/sahara_rt.png", "./tests/assets/env/sahara_lf.png",
																           "./tests/assets/env/sahara_up.png", "./tests/assets/env/sahara_dn.png",
																           "./tests/assets/env/sahara_ft.png", "./tests/assets/env/sahara_bk.png" );
			}
			else
			{
				cubemap_setup = tex_cube_map_setup( cube_map_texture[ i ], "./tests/assets/env/sorbin_rt.png", "./tests/assets/env/sorbin_lf.png",
																           "./tests/assets/env/sorbin_up.png", "./tests/assets/env/sorbin_dn.png",
																           "./tests/assets/env/sorbin_ft.png", "./tests/assets/env/sorbin_bk.png" );
			}

			if( !cubemap_setup )
			{
				fprintf( stderr, "[ERROR] Failed to setup sky box.\n" );
				exit( EXIT_FAILURE );
			}

			assert(check_gl() == GL_NO_ERROR);
		}
		else
		{
			dump_sdl_error( );
		}
	}



	glGenVertexArrays( 1, &vao );
	assert(check_gl() == GL_NO_ERROR);
	glBindVertexArray( vao );

	if( buffer_create( &vbo_vertices, cube_map_vertices, sizeof(GLfloat), sizeof(cube_map_vertices)/sizeof(cube_map_vertices[0]), GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		assert(check_gl() == GL_NO_ERROR);
		glEnableVertexAttribArray( attribute_vertex );
		assert(check_gl() == GL_NO_ERROR);
		glVertexAttribPointer( attribute_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		assert(check_gl() == GL_NO_ERROR);
		glDisableVertexAttribArray( attribute_vertex );
		assert(check_gl() == GL_NO_ERROR);
	}
	else
	{
		dump_sdl_error( );
	}


	glPointSize( 1.0 );



	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	glViewport(0, 0, width, height );
	assert(check_gl() == GL_NO_ERROR);


	camera = camera_create( width, height, 2.0f, 1000.0f, 45.0f, &VEC3(0.0f, 0.0f, 0.0f) );
	assert( camera );
}

void deinitialize( void )
{
	for( int i = 0; i < 3; i++ )
	{
		tex_destroy( cube_map_texture[ i ] );
	}
	glDeleteVertexArrays( 1, &vao );
	glDeleteBuffers( 1, &vbo_vertices );
	glDeleteBuffers( 1, &vbo_tex_coords );
	glDeleteBuffers( 1, &ibo_indices );
	glDeleteProgram( program );
	camera_destroy( camera );
	raster_font_destroy( font1 );
	raster_font_destroy( font2 );
}

void render( )
{
	GLuint now = SDL_GetTicks( );
	delta = frame_delta( now /* milliseconds */ );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	camera_update( camera, delta );
	assert(check_gl() == GL_NO_ERROR);

    render_skybox();
	assert(check_gl() == GL_NO_ERROR);

	assert( camera != NULL );

	raster_font_drawf( font2, &VEC2(2, 2 + 8 * 1.5f ), &VEC3(1,1,0), 1.5f, "Skybox" );
	raster_font_drawf( font1, &VEC2(2, 2), &VEC3(1,1,1), 1.0f, "FPS: %.1f", frame_rate(delta) );
	raster_font_drawf( font1, &VEC2(890, 2), &VEC3(0,1,1), 1.0f, "Press 1, 2, or 3." );
	assert(check_gl() == GL_NO_ERROR);

	SDL_GL_SwapWindow( window );
	//print_frame_rate( delta /* milliseconds */ );
}

void dump_sdl_error( void )
{
	const char* sdl_error = SDL_GetError( );

	if( sdl_error && *sdl_error != '\0' )
	{
		fprintf( stderr, "[SDL] %s\n", sdl_error );
	}
}

void render_skybox( void )
{
	glUseProgram( program );
	assert(check_gl() == GL_NO_ERROR);

	glBindVertexArray( vao );
	assert(check_gl() == GL_NO_ERROR);

	glEnableVertexAttribArray( attribute_vertex );
	assert(check_gl() == GL_NO_ERROR);
	glUniformMatrix4fv( uniform_projection, 1, GL_FALSE, (float*) camera_projection_matrix(camera) );
	assert(check_gl() == GL_NO_ERROR);
	glUniformMatrix4fv( uniform_orientation, 1, GL_FALSE, (float*) camera_orientation_matrix(camera) );
	assert(check_gl() == GL_NO_ERROR);


    glBindTexture( GL_TEXTURE_CUBE_MAP, cube_map_texture[ selected_skybox ] );
	assert(check_gl() == GL_NO_ERROR);
	glDepthMask( GL_FALSE );
	assert(check_gl() == GL_NO_ERROR);
	glDrawArrays( GL_TRIANGLES, 0, sizeof(cube_map_vertices)/sizeof(cube_map_vertices[0]) );
	assert(check_gl() == GL_NO_ERROR);
	glDepthMask( GL_TRUE );
	assert(check_gl() == GL_NO_ERROR);

	glDisableVertexAttribArray( attribute_vertex );
	assert(check_gl() == GL_NO_ERROR);
}

void set_view_by_mouse( )
{
	int screen_width;
	int screen_height;
	SDL_GetWindowSize( window, &screen_width, &screen_height );


	int middle_x = screen_width / 2; // the middle of the screen in the x direction
	int middle_y = screen_height / 2; // the middle of the screen in the y direction


	// the coordinates of our mouse coordinates
	int mouse_x;
	int mouse_y;
	// This function gets the position of the mouse
	SDL_GetMouseState( &mouse_x, &mouse_y );

	// if the mouse hasn't moved, return without doing
	// anything to our view
	if (mouse_x == middle_x && mouse_y == middle_y ) return;

	// otherwise move the mouse back to the middle of the screen
	SDL_WarpMouseInWindow( window, middle_x, middle_y );

	// get the distance and direction the mouse moved in x (in
	// pixels). We can't use the actual number of pixels in radians,
	// as only six pixels  would cause a full 360 degree rotation.
	// So we use a mousesensitivity variable that can be changed to
	// vary how many radians we want to turn in the x-direction for
	// a given mouse movement distance

	// We have to remember that positive rotation is counter-clockwise.
	// Moving the mouse down is a negative rotation about the x axis
	// Moving the mouse right is a negative rotation about the y axis
	// vector that describes mouseposition - center
	vec2_t mouse_direction = VEC2(
        (mouse_x - middle_x) * mouse_sensitivity,
		(mouse_y - middle_y) * mouse_sensitivity
	);

    if( scaler_abs(mouse_direction.x) > to_radians(0.05f) || scaler_abs(mouse_direction.y) > to_radians(0.05f) )
    {
        camera_offset_orientation( camera, mouse_direction.x, mouse_direction.y );
    }
}
