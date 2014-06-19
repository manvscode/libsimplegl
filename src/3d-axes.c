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

static const GLfloat axes_and_colors[] = {
	0.0f, 0.0f, 0.0f,         // origin
	1.0f, 0.0f, 0.0f,         // red
	1.0f, 0.0f, 0.0f,         // coordinate
	1.0f, 0.0f, 0.0f,         // red

	0.0f, 0.0f, 0.0f,         // origin
	0.0f, 1.0f, 0.0f,         // green
	0.0f, 1.0f, 0.0f,         // coordinate
	0.0f, 1.0f, 0.0f,         // green

	0.0f, 0.0f, 0.0f,         // origin
	0.0f, 0.0f, 1.0f,         // blue
	0.0f, 0.0f, 1.0f,         // coordinate
	0.0f, 0.0f, 1.0f,         // blue
};

static const char* vertex_shader_source = "#version 150\n"
                                          "in vec3 a_vertex;"
                                          "in vec3 a_color;"
                                          "out vec4 f_color;"
                                          "uniform mat4 u_model_view;"
                                          "void main( ) {"
                                          "		gl_Position = u_model_view * vec4( a_vertex, 1.0 );"
                                          "		f_color = vec4(a_color, 1);"
                                          "}";
static const char* fragment_shader_source = "#version 150\n"
                                            "in vec4 f_color;"
                                            "out vec4 color;"
                                            "void main( ) {"
	                                        "	color = f_color;"
                                            "}";

#define AXES_ARRAY_LENGTH         (sizeof(axes_and_colors)/sizeof(axes_and_colors[0]))

struct axes_3d {
	GLuint vao;
	GLuint vbo;
	GLuint program;
	GLint  attribute_vertex;
	GLint  attribute_color;
	GLint  uniform_model_view;
	GLfloat line_width;
};

axes_3d_t axes_3d_create( GLfloat line_width )
{
	GLuint program         = 0;
	GLuint vertex_shader   = 0;
	GLuint fragment_shader = 0;

	axes_3d_t axes = (axes_3d_t) malloc( sizeof(struct axes_3d) );

	if( axes )
	{
		#ifdef GL_LINE_SMOOTH
		GLfloat range[ 2 ];

		if( glIsEnabled( GL_LINE_SMOOTH ) )
		{
			glGetFloatv( GL_ALIASED_LINE_WIDTH_RANGE, range );

			if( line_width < range[ 0 ] || line_width > range[ 1 ] )
			{
				#ifdef SIMPLEGL_DEBUG
				fprintf( stdout, "[GL] Line width must be within [%f, %f]\n", range[0], range[1] );
				#endif
				goto failure;
			}
		}
		else
		{
			glGetFloatv( GL_SMOOTH_LINE_WIDTH_RANGE, range );

			if( line_width < range[ 0 ] || line_width > range[ 1 ] )
			{
				#ifdef SIMPLEGL_DEBUG
				fprintf( stdout, "[GL] Line width must be within [%f, %f]\n", range[0], range[1] );
				#endif
				goto failure;
			}
		}
		#endif

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
		assert(check_gl() == GL_NO_ERROR);

		if( !glsl_link_program( program ) )
		{
			goto failure;
		}

		glsl_destroy( vertex_shader );
		glsl_destroy( fragment_shader );

		GLint attribute_vertex   = glsl_bind_attribute( program, "a_vertex" );
		GLint attribute_color    = glsl_bind_attribute( program, "a_color" );
		GLint uniform_model_view = glsl_bind_uniform( program, "u_model_view" );
		assert(check_gl() == GL_NO_ERROR);

		if( buffer_create( &axes->vbo, axes_and_colors, sizeof(GLfloat), AXES_ARRAY_LENGTH, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
		{
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenVertexArrays( 1, &axes->vao );
			glBindVertexArray( axes->vao );

			glBindBuffer( GL_ARRAY_BUFFER, axes->vbo );

			glEnableVertexAttribArray( attribute_vertex );
			glVertexAttribPointer( attribute_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0 );
			//glDisableVertexAttribArray( attribute_vertex );

			glEnableVertexAttribArray( attribute_color );
			glVertexAttribPointer( attribute_color, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (const void*) (sizeof(GLfloat) * 3) );
			//glDisableVertexAttribArray( attribute_color );
			assert(check_gl() == GL_NO_ERROR);

			glBindVertexArray( 0 );
		}
		else
		{
			goto failure;
		}

		axes->program            = program;
		axes->attribute_vertex   = attribute_vertex;
		axes->attribute_color    = attribute_color;
		axes->uniform_model_view = uniform_model_view;
		axes->line_width         = line_width;
	}

	return axes;

failure:
	#ifdef SIMPLEGL_DEBUG
	fprintf( stdout, "[GL] Unable to create 3D axes\n" );
	#endif
	if( vertex_shader ) glsl_destroy( vertex_shader );
	if( fragment_shader ) glsl_destroy( fragment_shader );
	if( program ) glsl_destroy( program );
	if( axes ) free( axes );
	return NULL;
}

void axes_3d_destroy( axes_3d_t* axes )
{
	if( *axes )
	{
		glDeleteProgram( (*axes)->program );
		glDeleteVertexArrays( 1, &(*axes)->vao );
		glDeleteBuffers( 1, &(*axes)->vbo );
		free( *axes );
		*axes = NULL;
	}
}

void axes_3d_render( axes_3d_t axes, const GLfloat* model_view )
{
	GLfloat current_line_width;
	glGetFloatv( GL_LINE_WIDTH, &current_line_width );
	glLineWidth( axes->line_width );
	assert(check_gl() == GL_NO_ERROR);

	glUseProgram( axes->program );
	assert(check_gl() == GL_NO_ERROR);

	glBindVertexArray( axes->vao );
		//glEnableVertexAttribArray( axes->attribute_vertex );
		//glEnableVertexAttribArray( axes->attribute_color );
		assert(check_gl() == GL_NO_ERROR);
		glUniformMatrix4fv( axes->uniform_model_view, 1, GL_FALSE, model_view );

		glDrawArrays( GL_LINES, 0, AXES_ARRAY_LENGTH );
		assert(check_gl() == GL_NO_ERROR);
		//glDisableVertexAttribArray( axes->attribute_vertex );
		//glDisableVertexAttribArray( axes->attribute_color );

	glBindVertexArray( 0 );
	assert(check_gl() == GL_NO_ERROR);

	glLineWidth( current_line_width );
}

