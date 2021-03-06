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
#include <m3d/vec3.h>
#include <m3d/vec4.h>
#include <m3d/quat.h>
#include <SDL2/SDL.h>

static void initialize     ( void );
static void deinitialize   ( void );
static void render         ( void );
static void dump_sdl_error ( void );

SDL_Window* window = NULL;
SDL_GLContext ctx = NULL;

GLuint program = 0;
GLint uniform_model_view = 0;
GLint uniform_texture = 0;

GLuint vao = 0;
GLuint vbo_vertices = 0;
GLuint vbo_colors = 0;
GLuint vbo_tex_coords = 0;
GLuint ibo_indices = 0;
GLuint checkered_pattern_texture = 0;
GLuint subtitle_texture_overlay;
polyhedra_t polyhedra;
raster_font_t* font1 = NULL;
raster_font_t* font2 = NULL;
quat_t cube_orientation = QUAT(0,1,0,0);

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
	window = SDL_CreateWindow( "Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags );

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



void (*__glPushGroupMarker)( GLsizei length, const GLchar* marker ) = NULL;
void (*__glPopGroupMarker)( ) = NULL;
void (*__glLabelObject)( GLenum type, GLuint object, GLsizei length, const GLchar *label ) = NULL;
void (*__glGetObjectLabel)( GLenum type, GLuint object, GLsizei bufSize, GLsizei *length, GLchar *label ) = NULL;

void initialize( void )
{

	if( gl_has_extension( "GL_EXT_debug_marker" ) )
	{
		__glPushGroupMarker = gl_extension( "glPushGroupMarkerEXT" );
		__glPopGroupMarker  = gl_extension( "glPopGroupMarkerEXT" );
	}

	if( gl_has_extension( "GL_EXT_debug_label" ) )
	{
		__glLabelObject     = gl_extension( "glLabelObjectEXT" );
		__glGetObjectLabel  = gl_extension( "glGetObjectLabelEXT" );
	}

	gl_info_print( );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
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
		{ GL_VERTEX_SHADER,   "assets/shaders/cube.v.glsl" },
		{ GL_FRAGMENT_SHADER, "assets/shaders/cube.f.glsl" }
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

	if( __glLabelObject )
	{
		__glLabelObject( GL_PROGRAM_OBJECT_EXT, program, 0, "Cube Shader Program" );
	}

	uniform_model_view = glsl_bind_uniform( program, "u_model_view" );
	GL_ASSERT_NO_ERROR( );
	uniform_texture = glsl_bind_uniform( program, "u_texture" );
	GL_ASSERT_NO_ERROR( );

	checkered_pattern_texture = tex_create( );
	GL_ASSERT_NO_ERROR( );
	if( checkered_pattern_texture )
	{
		glActiveTexture( GL_TEXTURE0 );
		GL_ASSERT_NO_ERROR( );
		tex_load_2d_with_mipmaps( checkered_pattern_texture, "assets/textures/checkered.png", TEX_CLAMP_S | TEX_CLAMP_T );
		GL_ASSERT_NO_ERROR( );
		if( __glLabelObject )
		{
			__glLabelObject( GL_TEXTURE, checkered_pattern_texture, 0, "Cube Texture" );
		}
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

	polyhedra_create( &polyhedra );


	#if 0
	tetrahedron( &polyhedra, 3.0f, true );
	#else
	cube( &polyhedra, 3.0f, true );
	#endif


	if( buffer_create( &vbo_vertices, polyhedra.vertices, sizeof(GLfloat), polyhedra.vertices_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		if( __glLabelObject )
		{
			__glLabelObject( GL_BUFFER_OBJECT_EXT, vbo_vertices, 0, "Cube Vertices VBO" );
		}
	}
	else
	{
		dump_sdl_error( );
		exit( EXIT_FAILURE );
	}

	if( buffer_create( &vbo_tex_coords, polyhedra.tex_coords, sizeof(GLfloat), polyhedra.tex_coords_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		if( __glLabelObject )
		{
			__glLabelObject( GL_BUFFER_OBJECT_EXT, vbo_tex_coords, 0, "Cube Texture Coordinates VBO" );
		}
	}
	else
	{
		dump_sdl_error( );
		exit( EXIT_FAILURE );
	}

	if( buffer_create( &vbo_colors, polyhedra.colors, sizeof(GLfloat), polyhedra.colors_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		if( __glLabelObject )
		{
			__glLabelObject( GL_BUFFER_OBJECT_EXT, vbo_colors, 0, "Cube Colors VBO" );
		}
	}
	else
	{
		dump_sdl_error( );
		exit( EXIT_FAILURE );
	}


	if( buffer_create( &ibo_indices, polyhedra.indices, sizeof(GLushort), polyhedra.indices_count, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		if( __glLabelObject )
		{
			__glLabelObject( GL_BUFFER_OBJECT_EXT, ibo_indices, 0, "Cube Vertex Indices VBO" );
		}
	}
	else
	{
		dump_sdl_error( );
		exit( EXIT_FAILURE );
	}

	glGenVertexArrays( 1, &vao );
	if( __glLabelObject )
	{
		__glLabelObject( GL_BUFFER_OBJECT_EXT, vao, 0, "Cube VAO" );
	}
	GL_ASSERT_NO_ERROR( );
	glBindVertexArray( vao );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	GL_ASSERT_NO_ERROR( );

	glBindBuffer( GL_ARRAY_BUFFER, vbo_vertices );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, vbo_tex_coords );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, vbo_colors );
	glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo_indices );

	glPointSize( 1.0 );

	overlay_initialize( );

	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	glViewport(0, 0, width, height );
	GL_ASSERT_NO_ERROR( );
}

void deinitialize( void )
{
	tex_destroy( checkered_pattern_texture );
	tex_destroy( subtitle_texture_overlay );
	glDeleteVertexArrays( 1, &vao );
	glDeleteBuffers( 1, &vbo_vertices );
	glDeleteBuffers( 1, &vbo_tex_coords );
	glDeleteBuffers( 1, &vbo_colors );
	glDeleteBuffers( 1, &ibo_indices );
	glDeleteProgram( program );
	polyhedra_destroy( &polyhedra );
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

	vec3_t translation = VEC3( 0.0, 0.0, -10 );
	#if 1
	mat4_t projection = perspective( 90.0 * M3D_RADIANS_PER_DEGREE, aspect, 1.0, 100.0 );
	#else
	mat4_t projection = orthographic( -10.0f * aspect, 10.0f * aspect, -10.0f, 10.0f, -20.0f, 20.0f );
	#endif
	const float angle = 0.05f;
	quat_t q = quat_from_axis3_angle( &VEC3(0, 1, 1), angle * M3D_RADIANS_PER_DEGREE * delta );
	cube_orientation = quat_multiply( &cube_orientation, &q );
	mat4_t rotation = quat_to_mat4( &cube_orientation );
	mat4_t transform = m3d_translate( &translation );
	transform = mat4_mult_matrix( &transform, &rotation );
	mat4_t model_view = mat4_mult_matrix( &projection, &transform );


	glUseProgram( program );
	glBindVertexArray( vao );
	assert(gl_error() == GL_NO_ERROR);
	glUniformMatrix4fv( uniform_model_view, 1, GL_FALSE, model_view.m );
	assert(gl_error() == GL_NO_ERROR);

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, checkered_pattern_texture );
	glUniform1i( uniform_texture, 0 );
	assert(gl_error() == GL_NO_ERROR);
	glDrawElements( GL_TRIANGLES, polyhedra.indices_count, GL_UNSIGNED_SHORT, 0 );
	assert(gl_error() == GL_NO_ERROR);

	glBindVertexArray( 0 );
	GL_ASSERT_NO_ERROR( );

	overlay_render( &VEC2(0,0), &VEC2(540,32), &VEC3(1,1,1), subtitle_texture_overlay );

	raster_font_shadowed_writef( font2, &VEC2(2, 2 + 8 * 1.5f ), &VEC3(1,1,0), &VEC3_ZERO, 0.8f, "Cube" );
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
