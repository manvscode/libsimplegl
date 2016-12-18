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
#include <SDL2/SDL.h>

static void initialize     ( void );
static void deinitialize   ( void );
static void render         ( void );
static void dump_sdl_error ( void );

SDL_Window* window = NULL;
SDL_GLContext ctx = NULL;

GLuint program = 0;
GLint uniform_model_view = 0;

GLuint vao = 0;
GLuint vbo_vertices = 0;
GLuint subtitle_texture_overlay;
raster_font_t* font1 = NULL;
raster_font_t* font2 = NULL;

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
	window = SDL_CreateWindow( "Triangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags );

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
		}
	}

	deinitialize( );

quit:
	if( ctx ) SDL_GL_DeleteContext( ctx );
	if( window ) SDL_DestroyWindow( window );
	SDL_Quit( );
	return 0;
}



void (*glPushGroupMarker)( GLsizei length, const GLchar* marker ) = NULL;
void (*glPopGroupMarker)( ) = NULL;
void (*glLabelObject)( GLenum type, GLuint object, GLsizei length, const GLchar *label ) = NULL;
void (*glGetObjectLabel)( GLenum type, GLuint object, GLsizei bufSize, GLsizei *length, GLchar *label ) = NULL;

void initialize( void )
{

	if( gl_has_extension( "GL_EXT_debug_marker" ) )
	{
		glPushGroupMarker = gl_extension( "glPushGroupMarkerEXT" );
		glPopGroupMarker  = gl_extension( "glPopGroupMarkerEXT" );
	}

	if( gl_has_extension( "GL_EXT_debug_label" ) )
	{
		glLabelObject     = gl_extension( "glLabelObjectEXT" );
		glGetObjectLabel  = gl_extension( "glGetObjectLabelEXT" );
	}

	gl_info_print( );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );

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
		{ GL_VERTEX_SHADER,   "assets/shaders/triangle.v.glsl" },
		{ GL_FRAGMENT_SHADER, "assets/shaders/triangle.f.glsl" }
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
	glLabelObject( GL_PROGRAM_OBJECT_EXT, program, 0, "Triangle Shader Program" );

	uniform_model_view = glsl_bind_uniform( program, "u_model_view" );
	GL_ASSERT_NO_ERROR( );

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

	float triangleVertices[] = {
	   1.0000000000000000f,   0.0000000000000000f,   0.0000000000000000f,
	   1.0000000000000000f,   0.0000000000000000f,   0.0000000000000000f,

	  -0.4999999999999999f,   0.8660254037844387f,   0.0000000000000000f,
	   0.0000000000000000f,   1.0000000000000000f,   0.0000000000000000f,

	  -0.5000000000000001f,  -0.8660254037844386f,   0.0000000000000000f,
	   0.0000000000000000f,   0.0000000000000000f,   1.0000000000000000f
	};


	if( buffer_create( &vbo_vertices, triangleVertices, sizeof(GLfloat), 6 * 3, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		glLabelObject( GL_BUFFER_OBJECT_EXT, vbo_vertices, 0, "Triangle Vertices and Colors VBO" );
	}
	else
	{
		dump_sdl_error( );
		exit( EXIT_FAILURE );
	}

	glGenVertexArrays( 1, &vao );
	glLabelObject( GL_BUFFER_OBJECT_EXT, vao, 0, "Triangle VAO" );
	GL_ASSERT_NO_ERROR( );
	glBindVertexArray( vao );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	GL_ASSERT_NO_ERROR( );

	glBindBuffer( GL_ARRAY_BUFFER, vbo_vertices );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*) 0 );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), ((GLvoid*)0) + 3 * sizeof(float) );


	glPointSize( 1.0 );

	overlay_initialize( );

	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	glViewport(0, 0, width, height );
	GL_ASSERT_NO_ERROR( );
}

void deinitialize( void )
{
	tex_destroy( subtitle_texture_overlay );
	glDeleteVertexArrays( 1, &vao );
	glDeleteBuffers( 1, &vbo_vertices );
	glDeleteProgram( program );
	raster_font_destroy( font1 );
	raster_font_destroy( font2 );
	overlay_deinitialize( );
}

GLuint delta = 0;
float angle = 0.0f;

void render( )
{
	GLuint now = SDL_GetTicks( );
	delta = frame_delta( now /* milliseconds */ );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	GLfloat aspect = ((GLfloat)width) / height;

	vec3_t translation = VEC3( 0.0, 0.0, -2 );
	#if 0
	mat4_t projection = perspective( 100.0 * RADIANS_PER_DEGREE, aspect, 1.0, 100.0 );
	#else
	mat4_t projection = orthographic( -2.0f * aspect, 2.0f * aspect, -2.0f, 2.0f, -10.0f, 10.0f );
	#endif

	mat4_t rotation = rotate_z( angle );
	mat4_t transform = translate( &translation );
	transform = mat4_mult_matrix( &transform, &rotation );
	mat4_t model_view = mat4_mult_matrix( &projection, &transform );

	if( angle > TWO_PI )
	{
		angle = 0.0f;
	}
	else
	{
		angle += 0.001f * delta;
	}

	glUseProgram( program );
	glBindVertexArray( vao );
	assert(gl_error() == GL_NO_ERROR);
	glUniformMatrix4fv( uniform_model_view, 1, GL_FALSE, model_view.m );
	assert(gl_error() == GL_NO_ERROR);

	glDrawArrays( GL_TRIANGLES, 0, 3 );
	assert(gl_error() == GL_NO_ERROR);

	glBindVertexArray( 0 );
	GL_ASSERT_NO_ERROR( );

	overlay_render( &VEC2(0,0), &VEC2(540,32), &VEC3(1,1,1), subtitle_texture_overlay );

	raster_font_shadowed_writef( font2, &VEC2(2, 2 + 8 * 1.5f ), &VEC3(1,1,0), &VEC3_ZERO, 0.8f, "Triangle" );
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
