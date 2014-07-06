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
#include <time.h>
#include <unistd.h>
#include "../src/simplegl.h"
#include <lib3dmath/vec3.h>
#include <lib3dmath/quat.h>
#include <SDL2/SDL.h>

#define RENDER_MODE_XRAY                      0u
#define RENDER_MODE_HIGH_INTENSITY_PROJECTION 1u
#define RENDER_PASS_BACK_VOXELS               0u
#define RENDER_PASS_SAMPLED_VOXELS            1u
#define MAX_VOLUMES                           6

static const char* volume_files[ MAX_VOLUMES ] = {
	"assets/volumes/head-256x256x256.raw",
	"assets/volumes/foot-256x256x256.raw",
	"assets/volumes/skull-256x256x256.raw",
	"assets/volumes/stent-512x512x174.raw",
	//"assets/volumes/lobster-301x324x56.raw",
	//"assets/volumes/stagbeetle-832x832x494.raw",
	"assets/volumes/carp-256x256x512-2.raw",
	"assets/volumes/virgo-256x256x256-3.raw",
};

static const vec4_t volume_dimensions[ MAX_VOLUMES ] = {
	VEC4( 256, 256, 256, 8 ),
	VEC4( 256, 256, 256, 8 ),
	VEC4( 256, 256, 256, 8 ),
	VEC4( 512, 512, 174, 8 ),
	//VEC4( 301, 324, 56, 8 ),
	//VEC4( 832, 832, 494, 16 ),
	VEC4( 256, 256, 512, 16 ),
	VEC4( 256, 256, 256, 24 ),
};


static void initialize     ( void );
static void deinitialize   ( void );
static void render         ( void );
static void dump_sdl_error ( void );
static void load_volumes   ( void );

SDL_Window* window = NULL;
SDL_GLContext ctx = NULL;

GLuint program = 0;
GLint attribute_vertex = 0;
GLint attribute_tex_coord = 0;
GLint attribute_color = 0;
GLint uniform_model_view = 0;
GLint uniform_voxel_data = 0;
GLint uniform_back_voxels = 0;
GLint uniform_color_transfer = 0;
GLint uniform_rendering_pass = 0;
GLint uniform_seed = 0;
GLint uniform_render_mode = 0;

GLuint vao = 0;
GLuint vbo_vertices = 0;
GLuint vbo_tex_coords = 0;
GLuint ibo_indices = 0;
GLuint voxel_texture[ MAX_VOLUMES ] = { 0 };
GLuint framebuffer = 0;
GLuint colorRenderBuffer;
GLuint back_voxel_texture;
GLuint color_transfer_texture;
GLuint color_transfer_texture_overlay;
GLuint subtitle_texture_overlay;

GLuint selected_volume = 0;
GLuint render_mode = 0;
raster_font_t* font1 = NULL;
raster_font_t* font2 = NULL;
GLfloat zoom = -6.0f;



//                       (-1,1,-1)    (1,1,-1)                            (0,1,0)    (1,1,0)
//                            +------+                                        +------+
//                          .'|    .'|                                      .'|    .'|
//               (-1,1,1) +---+--+'  | (1,1,1)                    (0,1,1) +---+--+'  | (1,1,1)
//                        |   |  |   |                                    |   |  |   |
//             (-1,-1,-1) |   +--+---+ (1,-1,-1)                  (0,0,0) |   +--+---+ (1,0,0)
//                        | .'   | .'                                     | .'   | .'
//                        +------+'                                       +------+'
//                 (-1,-1,1)     (1,-1,1)                          (0,0,1)     (1,0,1)

#define array_length( arr )   (sizeof(arr) / sizeof(arr[0]))

static GLfloat vertices[] = {
	// front
	-1, -1,  1,
	 1, -1,  1,
	 1,  1,  1,
	-1,  1,  1,
	// top
	-1,  1,  1,
	 1,  1,  1,
	 1,  1, -1,
	-1,  1, -1,
	// back
	 1, -1, -1,
	-1, -1, -1,
	-1,  1, -1,
	 1,  1, -1,
	// bottom
	-1, -1, -1,
	 1, -1, -1,
	 1, -1,  1,
	-1, -1,  1,
	// left
	-1, -1, -1,
	-1, -1,  1,
	-1,  1,  1,
	-1,  1, -1,
	// right
	 1, -1,  1,
	 1, -1, -1,
	 1,  1, -1,
	 1,  1,  1,
};
#define vertex_count array_length(vertices)

static GLfloat tex_coords[] = {
	// front
	0, 0, 1,
	1, 0, 1,
	1, 1, 1,
	0, 1, 1,
	// top
	0, 1, 1,
	1, 1, 1,
	1, 1, 0,
	0, 1, 0,
	// back
	1, 0, 0,
	0, 0, 0,
	0, 1, 0,
	1, 1, 0,
	// bottom
	0, 0, 0,
	1, 0, 0,
	1, 0, 1,
	0, 0, 1,
	// left
	0, 0, 0,
	0, 0, 1,
	0, 1, 1,
	0, 1, 0,
	// right
	1, 0, 1,
	1, 0, 0,
	1, 1, 0,
	1, 1, 1,
};

#define tex_coords_count array_length(tex_coords)

static GLushort indices[] = {
	// front
	0,  1,  2,
	2,  3,  0,
	// top
	4,  5,  6,
	6,  7,  4,
	// back
	8,  9, 10,
	10, 11,  8,
	// bottom
	12, 13, 14,
	14, 15, 12,
	// left
	16, 17, 18,
	18, 19, 16,
	// right
	20, 21, 22,
	22, 23, 20,
};

#define indices_count array_length(indices)

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
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 2 );

	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	window = SDL_CreateWindow( "Volume Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags );

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

	bool done = false;
	bool fullscreen = false;

	SDL_SetWindowFullscreen( window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
	SDL_ShowCursor( SDL_DISABLE );

	/* event loop */
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
		if( state[SDL_SCANCODE_X] )
		{
			render_mode = RENDER_MODE_XRAY;
		}
		if( state[SDL_SCANCODE_H] )
		{
			render_mode = RENDER_MODE_HIGH_INTENSITY_PROJECTION;
		}
		if( state[SDL_SCANCODE_1] )
		{
			selected_volume = 0;
		}
		if( state[SDL_SCANCODE_2] )
		{
			selected_volume = 1;
		}
		if( state[SDL_SCANCODE_3] )
		{
			selected_volume = 2;
		}
		if( state[SDL_SCANCODE_4] )
		{
			selected_volume = 3;
		}
		if( state[SDL_SCANCODE_5] )
		{
			selected_volume = 4;
		}
		if( state[SDL_SCANCODE_6] )
		{
			selected_volume = 5;
		}
		/*
		if( state[SDL_SCANCODE_7] )
		{
			selected_volume = 6;
		}
		if( state[SDL_SCANCODE_8] )
		{
			selected_volume = 7;
		}
		*/
		if( state[SDL_SCANCODE_A] )
		{
			zoom += 0.1f;
		}
		if( state[SDL_SCANCODE_Z] )
		{
			zoom += -0.1f;
		}

		render( );
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
	char cwd[ 256 ];
	getcwd( cwd, sizeof(cwd) );
	printf( "Current Working Dir.: %s\n", cwd );
	glClearColor( 0.17f, 0.17f, 0.17f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	font2 = raster_font_create( RASTER_FONT_FONT3_16X16 );
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

	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	glViewport( 0, 0, width, height );
	assert( gl_error() == GL_NO_ERROR );

	GLchar* shader_log  = NULL;
	GLchar* program_log = NULL;
	const shader_info_t shaders[] = {
		{ GL_VERTEX_SHADER,   "assets/shaders/volume-render.v.glsl" },
		{ GL_FRAGMENT_SHADER, "assets/shaders/volume-render.f.glsl" }
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

	attribute_vertex = glsl_bind_attribute( program, "a_vertex" ); assert( gl_error() == GL_NO_ERROR );
	attribute_tex_coord = glsl_bind_attribute( program, "a_tex_coord" ); assert( gl_error() == GL_NO_ERROR );
	attribute_color  = glsl_bind_attribute( program, "a_color" ); assert( gl_error() == GL_NO_ERROR );
	uniform_model_view = glsl_bind_uniform( program, "u_model_view" ); assert( gl_error() == GL_NO_ERROR );
	uniform_voxel_data = glsl_bind_uniform( program, "u_voxel_data" ); assert( gl_error() == GL_NO_ERROR );
	uniform_back_voxels = glsl_bind_uniform( program, "u_back_voxels" ); assert( gl_error() == GL_NO_ERROR );
	uniform_color_transfer = glsl_bind_uniform( program, "u_color_transfer" ); assert( gl_error() == GL_NO_ERROR );
	uniform_rendering_pass = glsl_bind_uniform( program, "u_render_pass" ); assert( gl_error() == GL_NO_ERROR );
	uniform_seed = glsl_bind_uniform( program, "u_seed" ); assert( gl_error() == GL_NO_ERROR );
	uniform_render_mode = glsl_bind_uniform( program, "u_render_mode" ); assert( gl_error() == GL_NO_ERROR );


	load_volumes( );


	color_transfer_texture = tex_create( );

	if( color_transfer_texture )
	{
		assert( gl_error() == GL_NO_ERROR );
		if( !tex_load_1d( color_transfer_texture, "assets/textures/intensity.png", GL_LINEAR, GL_LINEAR, TEX_CLAMP_S ) )
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

	color_transfer_texture_overlay = tex_create( );

	if( color_transfer_texture_overlay )
	{
		assert( gl_error() == GL_NO_ERROR );
		if( !tex_load_2d( color_transfer_texture_overlay, "assets/textures/intensity-overlay.png", GL_LINEAR, GL_LINEAR, TEX_CLAMP_S | TEX_CLAMP_T ) )
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

	glGenVertexArrays( 1, &vao );
	assert( gl_error() == GL_NO_ERROR );
	glBindVertexArray( vao );
	assert( gl_error() == GL_NO_ERROR );

	assert( vertex_count > 4 );
	assert( tex_coords_count > 4 );

	if( buffer_create( &vbo_vertices, vertices, sizeof(GLfloat), vertex_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		assert( gl_error() == GL_NO_ERROR );
		glEnableVertexAttribArray( attribute_vertex );
		assert( gl_error() == GL_NO_ERROR );
		glVertexAttribPointer( attribute_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		assert( gl_error() == GL_NO_ERROR );
	}
	else
	{
		dump_sdl_error( );
	}

	if( buffer_create( &vbo_tex_coords, tex_coords, sizeof(GLfloat), tex_coords_count, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		assert( gl_error() == GL_NO_ERROR );
		glEnableVertexAttribArray( attribute_tex_coord );
		assert( gl_error() == GL_NO_ERROR );
		glVertexAttribPointer( attribute_tex_coord, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		assert( gl_error() == GL_NO_ERROR );
	}
	else
	{
		dump_sdl_error( );
	}

	if( buffer_create( &ibo_indices, indices, sizeof(GLushort), indices_count, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW ) )
	{
		assert( gl_error() == GL_NO_ERROR );
	}
	else
	{
		dump_sdl_error( );
	}

	/* Create FBO */
	{
		glGenFramebuffers( 1, &framebuffer );
		glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );


		glGenRenderbuffers( 1, &colorRenderBuffer );
		glBindRenderbuffer( GL_RENDERBUFFER, colorRenderBuffer );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8, width, height );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer );
		assert( gl_error() == GL_NO_ERROR );


		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( status != GL_FRAMEBUFFER_COMPLETE )
		{
			printf( "Failed to make complete framebuffer object %x", status );
		}

		glGenTextures( 1, &back_voxel_texture );
		glBindTexture( GL_TEXTURE_2D, back_voxel_texture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, back_voxel_texture, 0 );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	glPointSize( 4.0 );

	overlay_initialize( );

	assert( gl_error() == GL_NO_ERROR );
}

void deinitialize( void )
{
	for( GLuint i = 0; i < MAX_VOLUMES; i++ )
	{
		tex_destroy( voxel_texture[ i ] );
	}
	tex_destroy( color_transfer_texture );
	tex_destroy( color_transfer_texture_overlay );
	tex_destroy( subtitle_texture_overlay );
	glDeleteFramebuffers( 1, &framebuffer );
	glDeleteRenderbuffers( 1, &colorRenderBuffer );
	glDeleteTextures( 1, &back_voxel_texture );
	glDeleteTextures( 1, &color_transfer_texture );
	glDeleteVertexArrays( 1, &vao );
	glDeleteBuffers( 1, &vbo_vertices );
	glDeleteBuffers( 1, &vbo_tex_coords );
	glDeleteBuffers( 1, &ibo_indices );
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
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	static float angle = -270.0;
	if( angle >= 360.0f ) angle = 0.0f;

	int width; int height;
	SDL_GetWindowSize( window, &width, &height );
	GLfloat aspect = ((GLfloat)width) / height;
	#if 1
	mat4_t projection = perspective( 30.0 * RADIANS_PER_DEGREE, aspect, 0.1, 100.0 );
	#else
	mat4_t projection = orthographic( -2.0f, 2.0f, -2.0f, 2.0f, -10.0f, 10.0f );
	#endif
	quat_t q1 = quat_from_axis3_angle( &VEC3( 0.0f, 1.0f, 0.0f ), angle * RADIANS_PER_DEGREE );
	angle += 0.025f * delta;
	mat4_t rotation_tfx  = quat_to_mat4( &q1 );
	const vec4_t* dimensions = &volume_dimensions[ selected_volume ];
	const scaler_t max_dimension = vec3_max_component( vec4_to_vec3( dimensions ) );
	const vec3_t scaled_dimension = VEC3(dimensions->x / max_dimension, dimensions->y / max_dimension, dimensions->z / max_dimension);

	mat4_t scale_tfx     = scale( &scaled_dimension );
	mat4_t translate_tfx = translate( &VEC3( 0.0, 0.0, zoom ) );
	mat4_t transform_tfx = MAT4_IDENTITY;
	transform_tfx = mat4_mult_matrix( &transform_tfx, &scale_tfx );
	transform_tfx = mat4_mult_matrix( &transform_tfx, &translate_tfx );
	transform_tfx = mat4_mult_matrix( &transform_tfx, &rotation_tfx );
	mat4_t model_view = mat4_mult_matrix( &projection, &transform_tfx );


	assert( gl_error() == GL_NO_ERROR );
	glUseProgram( program );

	glBindVertexArray( vao );

	glUniformMatrix4fv( uniform_model_view, 1, GL_FALSE, model_view.m );
	assert( gl_error() == GL_NO_ERROR );
	glUniform1i( uniform_voxel_data, 0 ); // texture unit 0 holds the voxel data
	assert( gl_error() == GL_NO_ERROR );
	glUniform1i( uniform_back_voxels, 1 ); // texture unit 1 holds the backing facing voxels
	assert( gl_error() == GL_NO_ERROR );
	glUniform1i( uniform_color_transfer, 2 ); // texture unit 2 holds the color transfer function
	assert( gl_error() == GL_NO_ERROR );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_3D, voxel_texture[ selected_volume ] );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, back_voxel_texture );
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_1D, color_transfer_texture );


	// First rendering pass
	glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
	glCullFace( GL_FRONT );
	glUniform1ui( uniform_rendering_pass, RENDER_PASS_BACK_VOXELS );
	glDrawElements( GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, 0 );
	glCullFace( GL_BACK );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// Second rendering pass
	glUniform1ui( uniform_rendering_pass, RENDER_PASS_SAMPLED_VOXELS );
	glUniform1ui( uniform_seed, now );
	glUniform1ui( uniform_render_mode, render_mode );
	glDrawElements( GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, 0 );
	assert( gl_error() == GL_NO_ERROR );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_3D, 0 );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_1D, 0 );

	overlay_render( &VEC2(20,height-40), &VEC2(200,10), &VEC3(1,1,1), color_transfer_texture_overlay );
	raster_font_shadowed_writef( font1, &VEC2(10, height-50), &VEC3(1,1,1), &VEC3_ZERO, 1.0f, "Low" );
	raster_font_shadowed_writef( font1, &VEC2(200, height-50), &VEC3(1,1,1), &VEC3_ZERO, 1.0f, "High" );

	assert( gl_error() == GL_NO_ERROR );

	overlay_render( &VEC2(0,0), &VEC2(540,32), &VEC3(1,1,1), subtitle_texture_overlay );
	raster_font_shadowed_writef( font2, &VEC2(2, 2 + 8 * 1.5f ), &VEC3(1,1,0), &VEC3(0.2,0.2f,0), 0.8f, "Volume Rendering: %s", strrchr( volume_files[selected_volume], '/' ) + 1 );
	raster_font_shadowed_writef( font1, &VEC2(2, 2), &VEC3(1,1,1), &VEC3_ZERO, 1.0f, "FPS: %.1f", frame_rate(delta) );
	raster_font_shadowed_writef( font1, &VEC2(150, 2), &VEC3(1,0.5,0.2), &VEC3_ZERO, 1.0f, render_mode == RENDER_MODE_XRAY ? "Using X-Ray Mode" : "Using HIP Mode" );
	raster_font_shadowed_writef( font1, &VEC2(350, 2), &VEC3(0,1,1), &VEC3_ZERO, 1.0f, "Press 1, 2, 3, 4, or 5." );

	assert( gl_error() == GL_NO_ERROR );
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

void load_volumes( void )
{
	for( GLuint i = 0; i < MAX_VOLUMES; i++ )
	{
		voxel_texture[ i ] = tex_create( );

		if( voxel_texture[ i ] )
		{
			glActiveTexture( GL_TEXTURE0 );
			assert( gl_error() == GL_NO_ERROR );
			int flags = TEX_CLAMP_S | TEX_CLAMP_T | TEX_CLAMP_R;

			if( tex_load_3d( voxel_texture[ i ], volume_files[ i ], volume_dimensions[ i ].w, volume_dimensions[ i ].x, volume_dimensions[ i ].y, volume_dimensions[ i ].z, GL_LINEAR, GL_LINEAR, flags ) )
			{
				printf( "Loaded %s (%.0fx%.0fx%.0f,voxel_depth=%.0f)\n", volume_files[ i ], volume_dimensions[ i ].x, volume_dimensions[ i ].y, volume_dimensions[ i ].z, volume_dimensions[ i ].w );
			}
			else
			{
				printf( "Unable to load %s\n", volume_files[ i ] );
				dump_sdl_error( );
				exit( EXIT_FAILURE );
			}

			assert( gl_error() == GL_NO_ERROR );
		}
		else
		{
			dump_sdl_error( );
		}
	}
}
