/* Copyright (C) 2013 by Joseph A. Marrero, http://www.manvscode.com/
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
#include <lib3dmath/quat.h>
#include <SDL2/SDL.h>

static void initialize     ( void );
static void deinitialize   ( void );
static void render         ( void );
static void dump_sdl_error ( void );

SDL_Window* window = NULL;
SDL_GLContext ctx = NULL;

GLuint program = 0;
GLint attribute_vertex = 0;
GLint attribute_tex_coord = 0;
GLint attribute_color = 0;
GLint uniform_model_view = 0;
GLint uniform_texture = 0;

GLuint vao = 0;
GLuint vbo_vertices = 0;
GLuint vbo_colors = 0;
GLuint vbo_tex_coords = 0;
GLuint ibo_indices = 0;
GLuint texture = 0;
polyhedra_t polyhedra;

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
	//flags |= SDL_WINDOW_FULLSCREEN;
	window = SDL_CreateWindow( "Test Shaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags );

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

	SDL_Event e;

	while( e.type != SDL_KEYDOWN && e.type != SDL_QUIT )
	{
		SDL_PollEvent( &e );      // Check for events.
		render( );
		//SDL_Delay(10);              // Pause briefly before moving on to the next cycle.
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
	dump_gl_info( );
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

	GLchar* shader_log  = NULL;
	GLchar* program_log = NULL;
	const shader_info_t shaders[] = {
		{ GL_VERTEX_SHADER,   "./tests/test-shaders.v.glsl" },
		{ GL_FRAGMENT_SHADER, "./tests/test-shaders.f.glsl" }
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

		return;
	}

	attribute_vertex = glsl_bind_attribute( program, "a_vertex" );
	GL_ASSERT_NO_ERROR( );
	attribute_tex_coord = glsl_bind_attribute( program, "a_tex_coord" );
	GL_ASSERT_NO_ERROR( );
	attribute_color  = glsl_bind_attribute( program, "a_color" );
	GL_ASSERT_NO_ERROR( );
	uniform_model_view = glsl_bind_uniform( program, "model_view" );
	GL_ASSERT_NO_ERROR( );
	uniform_texture = glsl_bind_uniform( program, "texture" );
	GL_ASSERT_NO_ERROR( );

	texture = tex2d_create( );
	GL_ASSERT_NO_ERROR( );
	if( texture )
	{
		glActiveTexture( GL_TEXTURE0 );
		GL_ASSERT_NO_ERROR( );
		tex2d_load_for_3d( texture, "./tests/checkered.png", true );
		GL_ASSERT_NO_ERROR( );
  		//glBindTexture( GL_TEXTURE_2D, texture_id );
  		//glUniform1i( uniform_texture, /*GL_TEXTURE*/ 0 );
		GL_ASSERT_NO_ERROR( );
	}
	else
	{
		dump_sdl_error( );
	}

	polyhedra_create( &polyhedra );


	#if 0
	tetrahedron( &polyhedra, 3.0f );
	#else
	cube( &polyhedra, 3.0f );
	#endif

	glGenVertexArrays( 1, &vao );
	GL_ASSERT_NO_ERROR( );
	glBindVertexArray( vao );

	if( buffer_create( &vbo_vertices, polyhedra.vertices, sizeof(GLfloat), polyhedra.vertices_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		GL_ASSERT_NO_ERROR( );
		glEnableVertexAttribArray( attribute_vertex );
		GL_ASSERT_NO_ERROR( );
		glVertexAttribPointer( attribute_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		GL_ASSERT_NO_ERROR( );
		glDisableVertexAttribArray( attribute_vertex );
		GL_ASSERT_NO_ERROR( );
	}
	else
	{
		dump_sdl_error( );
	}

	if( buffer_create( &vbo_tex_coords, polyhedra.tex_coords, sizeof(GLfloat), polyhedra.tex_coords_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		GL_ASSERT_NO_ERROR( );
		glEnableVertexAttribArray( attribute_tex_coord );
		GL_ASSERT_NO_ERROR( );
		glVertexAttribPointer( attribute_tex_coord, 2, GL_FLOAT, GL_FALSE, 0, 0 );
		GL_ASSERT_NO_ERROR( );
		glDisableVertexAttribArray( attribute_tex_coord );
		GL_ASSERT_NO_ERROR( );
	}
	else
	{
		dump_sdl_error( );
	}

	if( buffer_create( &vbo_colors, polyhedra.colors, sizeof(GLfloat), polyhedra.colors_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		GL_ASSERT_NO_ERROR( );
		glEnableVertexAttribArray( attribute_color );
		GL_ASSERT_NO_ERROR( );
		glVertexAttribPointer( attribute_color, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		GL_ASSERT_NO_ERROR( );
		glDisableVertexAttribArray( attribute_color );
		GL_ASSERT_NO_ERROR( );
	}
	else
	{
		dump_sdl_error( );
	}


	if( buffer_create( &ibo_indices, polyhedra.indices, sizeof(GLushort), polyhedra.indices_count, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
	}
	else
	{
		dump_sdl_error( );
	}

	glPointSize( 1.0 );



	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	glViewport(0, 0, width, height );
	GL_ASSERT_NO_ERROR( );
}

void deinitialize( void )
{
	tex2d_destroy( texture );
	glDeleteVertexArrays( 1, &vao );
	glDeleteBuffers( 1, &vbo_vertices );
	glDeleteBuffers( 1, &vbo_tex_coords );
	glDeleteBuffers( 1, &vbo_colors );
	glDeleteBuffers( 1, &ibo_indices );
	glDeleteProgram( program );
	polyhedra_destroy( &polyhedra );
}

void render( )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	static float angle = 0.0;
	if( angle >= 360.0f ) angle = 0.0f;
	#if 0
	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	GLfloat aspect = ((GLfloat)width) / height;
	vec3_t translation = VEC3_LITERAL( 0.0, 0.0, -10 );
	mat4_t projection = orthographic( -10.0, 10.0, -6.0*aspect, 6.0*aspect, -100.0, 100.0 );
	mat4_t rotation = rotate_xyz( "yx", angle, -5.0 );
	angle += 0.1f;
	mat4_t transform = translate( &translation );
	transform = mat4_mult_matrix( &transform, &rotation );
	mat4_t model_view = mat4_mult_matrix( &projection, &transform );
	#else
	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	GLfloat aspect = ((GLfloat)height) / width;
	vec3_t translation = VEC3_LITERAL( 0.0, 0.0, -10 );
	mat4_t projection = perspective( 60.0, aspect, 0.1, 100.0 );
	vec3_t axis = VEC3_LITERAL( -0.5f, -0.25f, 0.75f );
	quat_t q1 = quat_from_axis3_angle( &axis, angle );
	mat4_t rotation = quat_to_mat4( &q1 );
	angle += 0.003;
	mat4_t transform = translate( &translation );
	transform = mat4_mult_matrix( &transform, &rotation );
	mat4_t model_view = mat4_mult_matrix( &projection, &transform );
	#endif


	glUseProgram( program );

	glEnableVertexAttribArray( attribute_vertex );
	glEnableVertexAttribArray( attribute_tex_coord );
	glEnableVertexAttribArray( attribute_color );
	glUniformMatrix4fv( uniform_model_view, 1, GL_FALSE, model_view.m );


	glBindTexture( GL_TEXTURE_2D, texture );
	glBindVertexArray( vao );
	glDrawElements( GL_TRIANGLES, polyhedra.indices_count, GL_UNSIGNED_SHORT, 0 );


	glDisableVertexAttribArray( attribute_vertex );
	glDisableVertexAttribArray( attribute_tex_coord );
	glDisableVertexAttribArray( attribute_color );


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
