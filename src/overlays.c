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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libimageio/imageio.h>
#include "simplegl.h"
#include "raster-fonts.h"

typedef struct overlay_shader {
	GLuint program;
	GLint attribute_vertex;
	GLint uniform_color;
	GLint uniform_projection;
	GLint uniform_position;
	GLint uniform_width;
	GLint uniform_height;
	GLint uniform_texture;
	GLint uniform_use_texture;
	GLuint vao;
	GLuint vbo_mesh;

	GLuint overlay_count;
} overlay_shader_t;

overlay_shader_t overlay_shader = {
	.program = 0,
	.attribute_vertex = 0,
	.uniform_color = 0,
	.uniform_position = 0,
	.uniform_width = 0,
	.uniform_height = 0,
	.uniform_texture = 0,
	.uniform_use_texture = 0,
	.vao = 0,
	.vbo_mesh = 0,
	.overlay_count = 0
};

static const GLfloat overlay_vertices[] = {
	 0.0f,  0.0f,
	 1.0f,  0.0f,
	 0.0f,  1.0f,
	 0.0f,  1.0f,
	 1.0f,  0.0f,
	 1.0f,  1.0f,
};
#define OVERLAY_MESH_LENGTH         (sizeof(overlay_vertices)/sizeof(overlay_vertices[0]))

bool overlay_initialize( void )
{
	bool result = false;
	GLuint program = 0;
	GLuint vertex_shader = 0;
	GLuint fragment_shader = 0;
	GLint attribute_vertex = 0;
	GLint uniform_projection = 0;
	GLint uniform_position = 0;
	GLint uniform_color = 0;
	GLint uniform_width = 0;
	GLint uniform_height = 0;
	GLint uniform_texture = 0;
	GLint uniform_use_texture = 0;
	GLuint vao = 0;
	GLuint vbo_mesh = 0;

	if( overlay_shader.overlay_count == 0 )
	{
		program = glsl_create( GL_PROGRAM );

		if( !program )
		{
			goto failure;
		}

		vertex_shader = glsl_create( GL_VERTEX_SHADER );

		if( !vertex_shader )
		{
			goto failure;
		}

		#if TARGET_OS_IPHONE
		char* vertex_shader_source = glsl_shader_load( "assets/shaders/overlay-100.v.glsl" );
		#else
		char* vertex_shader_source = glsl_shader_load( "assets/shaders/overlay-150.v.glsl" );
		#endif
		if( !glsl_shader_compile( vertex_shader, vertex_shader_source ) )
		{
			free( vertex_shader_source );
			goto failure;
		}
		free( vertex_shader_source );

		fragment_shader = glsl_create( GL_FRAGMENT_SHADER );

		if( !fragment_shader )
		{
			goto failure;
		}

		#if TARGET_OS_IPHONE
		char* fragment_shader_source = glsl_shader_load( "assets/shaders/overlay-100.f.glsl" );
		#else
		char* fragment_shader_source = glsl_shader_load( "assets/shaders/overlay-150.f.glsl" );
		#endif
		if( !glsl_shader_compile( fragment_shader, fragment_shader_source ) )
		{
			free( fragment_shader_source );
			goto failure;
		}
		free( fragment_shader_source );

		glsl_attach_shader( program, vertex_shader );
		glsl_attach_shader( program, fragment_shader );
		assert( gl_error() == GL_NO_ERROR );

		if( !glsl_link_program( program ) )
		{
			goto failure;
		}

		glsl_destroy( vertex_shader );
		glsl_destroy( fragment_shader );

		attribute_vertex     = glsl_bind_attribute( program, "a_vertex" );
		uniform_color        = glsl_bind_uniform( program, "u_color" );
		uniform_projection   = glsl_bind_uniform( program, "u_projection" );
		uniform_position     = glsl_bind_uniform( program, "u_position" );
		uniform_width        = glsl_bind_uniform( program, "u_width" );
		uniform_height       = glsl_bind_uniform( program, "u_height" );
		uniform_texture      = glsl_bind_uniform( program, "u_texture" );
		uniform_use_texture  = glsl_bind_uniform( program, "u_use_texture" );
		assert( gl_error() == GL_NO_ERROR );

		glGenVertexArrays( 1, &vao );
		if( !vao ) goto failure;
		glBindVertexArray( vao );

		if( buffer_create( &vbo_mesh, overlay_vertices, sizeof(GLfloat), OVERLAY_MESH_LENGTH, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
		{
			glEnableVertexAttribArray( attribute_vertex );
			glVertexAttribPointer( attribute_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0 );
			assert( gl_error() == GL_NO_ERROR );
		}
		else
		{
			goto failure;
		}

		overlay_shader.program              = program;
		overlay_shader.attribute_vertex     = attribute_vertex;
		overlay_shader.uniform_color        = uniform_color;
		overlay_shader.uniform_projection   = uniform_projection;
		overlay_shader.uniform_position     = uniform_position;
		overlay_shader.uniform_width        = uniform_width;
		overlay_shader.uniform_height       = uniform_height;
		overlay_shader.uniform_texture      = uniform_texture;
		overlay_shader.uniform_use_texture  = uniform_use_texture;
		overlay_shader.vao                  = vao;
		overlay_shader.vbo_mesh             = vbo_mesh;
	}

	overlay_shader.overlay_count += 1;
	result = true;
	assert( gl_error() == GL_NO_ERROR );

	return result;

failure:
	if( vertex_shader ) glsl_destroy( vertex_shader );
	if( fragment_shader ) glsl_destroy( fragment_shader );
	if( program ) glsl_destroy( program );
	if( vao ) glDeleteVertexArrays( 1, &vao );

	return result;
}

void overlay_deinitialize( void )
{
	if( overlay_shader.overlay_count > 0 )
	{
		if( overlay_shader.overlay_count == 1 )
		{
			glsl_destroy( overlay_shader.program );
			glDeleteVertexArrays( 1, &overlay_shader.vao );
			glDeleteBuffers( 1, &overlay_shader.vbo_mesh );
		}

		overlay_shader.overlay_count -= 1;
	}
}

void overlay_render( const vec2_t* position, const vec2_t* size, const vec3_t* color, GLuint texture_2d )
{
	assert( gl_error() == GL_NO_ERROR );
	GLint viewport[ 4 ];

	glGetIntegerv( GL_VIEWPORT, viewport );

	mat4_t projection = orthographic( 0, viewport[2], 0, viewport[3], -10.0f, 10.0f );

	glDepthMask( GL_FALSE );

	glUseProgram( overlay_shader.program );
	assert(		gl_error() == GL_NO_ERROR );

	glBindVertexArray( overlay_shader.vao );

	assert( gl_error() == GL_NO_ERROR );

	glUniformMatrix4fv(overlay_shader.uniform_projection, 1, GL_FALSE, projection.m );
	glUniform3fv( overlay_shader.uniform_color, 1, (GLfloat*) color );
	assert( gl_error() == GL_NO_ERROR );

	if( texture_2d )
	{
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture_2d );
		assert( gl_error() == GL_NO_ERROR );
		glUniform1i( overlay_shader.uniform_use_texture, 1 );
		assert( gl_error() == GL_NO_ERROR );
	}
	else
	{
		glUniform1i( overlay_shader.uniform_use_texture, 0 );
		assert( gl_error() == GL_NO_ERROR );
	}

	glUniform1i( overlay_shader.uniform_texture, 0 );
	assert( gl_error() == GL_NO_ERROR );

	glUniform1i( overlay_shader.uniform_width, size->x );
	assert( gl_error() == GL_NO_ERROR );
	glUniform1i( overlay_shader.uniform_height, size->y );
	assert( gl_error() == GL_NO_ERROR );

	glUniform3fv( overlay_shader.uniform_position, 1, (GLfloat*) &VEC3( position->x, position->y, 0.0f ) );
	assert( gl_error() == GL_NO_ERROR );
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	assert( gl_error() == GL_NO_ERROR );

	assert( gl_error() == GL_NO_ERROR );

	glDepthMask( GL_TRUE );
	assert( gl_error() == GL_NO_ERROR );

	glBindVertexArray( 0 );
}
