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
#include <string.h>
#include <assert.h>
#include "simplegl.h"

static const char* vertex_shader_source = "#version 150\n"
                                          "in vec3 a_vertex;"
                                          "out vec4 f_color;"
                                          "uniform mat4 u_model_view;"
                                          "uniform vec4 u_axis_color;"
                                          "uniform vec4 u_color;"
                                          "void main( ) {"
                                          "		gl_Position = u_model_view * vec4( a_vertex, 1.0 );"
                                          "		f_color = (abs(a_vertex.x) < 0.00001 || abs(a_vertex.z) < 0.00001) ? u_axis_color : u_color;"
                                          "}";
static const char* fragment_shader_source = "#version 150\n"
                                            "in vec4 f_color;"
                                            "out vec4 color;"
                                            "void main( ) {"
	                                        "	color = f_color;"
                                            "}";

struct grid {
	GLuint vao;
	GLuint vbo;
	GLuint program;
	GLint  attribute_vertex;
	GLint  uniform_model_view;
	GLint uniform_axis_color;
	GLint uniform_color;
	GLsizei vertex_count;
	GLfloat size;
	GLfloat step;
};

grid_t grid_create( GLfloat size, GLfloat step )
{
	GLuint program         = 0;
	GLuint vertex_shader   = 0;
	GLuint fragment_shader = 0;

	grid_t grid = (grid_t) malloc( sizeof(struct grid) );

	if( grid )
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

		if( !glsl_shader_compile( vertex_shader, vertex_shader_source ) )
		{
			goto failure;
		}

		fragment_shader = glsl_create( GL_FRAGMENT_SHADER );

		if( !fragment_shader )
		{
			goto failure;
		}

		if( !glsl_shader_compile( fragment_shader, fragment_shader_source ) )
		{
			goto failure;
		}

		glsl_attach_shader( program, vertex_shader );
		glsl_attach_shader( program, fragment_shader );
		assert(gl_error() == GL_NO_ERROR);

		if( !glsl_link_program( program ) )
		{
			goto failure;
		}

		glsl_destroy( vertex_shader );
		glsl_destroy( fragment_shader );

		GLint attribute_vertex   = glsl_bind_attribute( program, "a_vertex" );
		assert(gl_error() == GL_NO_ERROR);
		GLint uniform_model_view = glsl_bind_uniform( program, "u_model_view" );
		assert(gl_error() == GL_NO_ERROR);
		GLint uniform_axis_color = glsl_bind_uniform( program, "u_axis_color" );
		assert(gl_error() == GL_NO_ERROR);
		GLint uniform_color      = glsl_bind_uniform( program, "u_color" );
		assert(gl_error() == GL_NO_ERROR);

		const GLsizei vertex_count = (GLsizei) 4 * ((2 * size) / step + 1);
		GLsizei vertex_i = 0;
		vec3_t grid_mesh[ vertex_count ];

		for( GLfloat i = -size; i <= size; i += step )
		{
			grid_mesh[ vertex_i + 0 ] = VEC3( -size, 0, i );
			grid_mesh[ vertex_i + 1 ] = VEC3(  size, 0, i );
			grid_mesh[ vertex_i + 2 ] = VEC3(     i, 0, -size );
			grid_mesh[ vertex_i + 3 ] = VEC3(     i, 0,  size );

			vertex_i += 4;
		}

		if( buffer_create( &grid->vbo, grid_mesh, sizeof(GLfloat), vertex_count * 3, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
		{
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenVertexArrays( 1, &grid->vao );
			glBindVertexArray( grid->vao );

			glBindBuffer( GL_ARRAY_BUFFER, grid->vbo );

			glEnableVertexAttribArray( attribute_vertex );
			glVertexAttribPointer( attribute_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0 );

			glBindVertexArray( 0 );
		}
		else
		{
			goto failure;
		}

		grid->program            = program;
		grid->attribute_vertex   = attribute_vertex;
		grid->uniform_model_view = uniform_model_view;
		grid->uniform_axis_color = uniform_axis_color;
		grid->uniform_color      = uniform_color;
		grid->vertex_count       = vertex_count;
		grid->size               = size;
		grid->step               = step;
	}

	return grid;

failure:
	#ifdef SIMPLEGL_DEBUG
	fprintf( stdout, "[GL] Unable to create 3D grid\n" );
	#endif
	if( vertex_shader ) glsl_destroy( vertex_shader );
	if( fragment_shader ) glsl_destroy( fragment_shader );
	if( program ) glsl_destroy( program );
	if( grid ) free( grid );
	return NULL;
}

void grid_destroy( grid_t* grid )
{
	if( *grid )
	{
		glDeleteProgram( (*grid)->program );
		glDeleteVertexArrays( 1, &(*grid)->vao );
		glDeleteBuffers( 1, &(*grid)->vbo );
		free( *grid );
		*grid = NULL;
	}
}

void grid_render( grid_t grid, const mat4_t* model_view, const vec4_t* primary_color, const vec4_t* secondary_color )
{
	GLfloat current_line_width;
	glGetFloatv( GL_LINE_WIDTH, &current_line_width );
	assert(gl_error() == GL_NO_ERROR);

	glUseProgram( grid->program );
	assert(gl_error() == GL_NO_ERROR);

	glBindVertexArray( grid->vao );
		assert(gl_error() == GL_NO_ERROR);
		glUniformMatrix4fv( grid->uniform_model_view, 1, GL_FALSE, model_view->m );
		glUniform4fv( grid->uniform_axis_color, 1, (GLfloat*) primary_color );
		glUniform4fv( grid->uniform_color, 1, (GLfloat*) secondary_color );

		glDrawArrays( GL_LINES, 0, grid->vertex_count );
		assert(gl_error() == GL_NO_ERROR);

	glBindVertexArray( 0 );
	assert(gl_error() == GL_NO_ERROR);
}

