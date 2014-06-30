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

#define RASTER_FONT_MAX_STRLEN    (256)
#define RASTER_FONT_MAX_FONTS     (4)

static const char* vertex_shader_source =
	"#version 150\n"
	"in vec3 a_vertex;"
	"out vec3 f_vertex;"
	"uniform mat4 u_projection;"
	"uniform vec3 u_position;"
	"uniform uint u_width;"
	"uniform uint u_height;"
	"uniform uint u_glyph_width;"
	"uniform uint u_glyph_height;"
	"uniform float u_size;"
	"void main( ) {"
	"    mat3 scale = mat3("
	"        u_glyph_width,              0, 0,"
	"        0, u_glyph_height, 0,"
	"        0,              0, 1"
	"    );"
	"    vec3 scaled_vertex = u_size * scale * a_vertex;"
	"    vec4 character_position = vec4( u_position, 0 ) + vec4( scaled_vertex, 1 );"
	"    gl_Position = u_projection * character_position;"
	"    f_vertex = a_vertex;"
	"}";
static const char* fragment_shader_source =
	"#version 150\n"
	"in vec3 f_vertex;"
	"out vec4 color;"
	"uniform vec3 u_color;"
	"uniform sampler2D u_texture;"
	"uniform uint u_width;"
	"uniform uint u_height;"
	"uniform uint u_glyph_width;"
	"uniform uint u_glyph_height;"
	"uniform uint u_character;"
	"void main( ) {"
	"    vec2 offset = vec2( 0, 1 - ((u_character + 1u) * u_glyph_height / float(u_height)) );"
	"    mat2 scale = mat2( "
	"        u_glyph_width / float(u_width),                         0,"
	"        0,                       u_glyph_height / float(u_height)"
	"    );"
	"    vec2 texture_coord = offset + scale * vec2( f_vertex.x, f_vertex.y );"
	"    vec4 texel = texture( u_texture, texture_coord );"
	"    color = vec4(u_color, 1) * vec4( texel.r );"
	"}";

typedef struct raster_font_shader {
	GLuint program;
	GLint attribute_vertex;
	GLint uniform_color;
	GLint uniform_projection;
	GLint uniform_position;
	GLint uniform_size;
	GLint uniform_glyph_width;
	GLint uniform_glyph_height;
	GLint uniform_width;
	GLint uniform_height;
	GLint uniform_character;
	GLint uniform_texture;
	GLuint vao;
	GLuint vbo_mesh;

	GLuint font_count;
} raster_font_shader_t;

raster_font_shader_t font_shader = {
	.program = 0,
	.attribute_vertex = 0,
	.uniform_color = 0,
	.uniform_position = 0,
	.uniform_size = 0,
	.uniform_glyph_width = 0,
	.uniform_glyph_height = 0,
	.uniform_width = 0,
	.uniform_height = 0,
	.uniform_character = 0,
	.uniform_texture = 0,
	.vao = 0,
	.vbo_mesh = 0,
	.font_count = 0
};

static bool raster_font_shader_initialize( raster_font_shader_t* shader );
static void raster_font_shader_deinitialize( raster_font_shader_t* shader );
static GLubyte* raster_font_expand( raster_font_t* font );

struct raster_font {
	const GLushort width;
	const GLushort height;
	const GLushort glyph_count;
	const GLushort glyph_width;
	const GLushort glyph_height;
	const GLushort bits_per_pixel;
	const GLushort size;
	const GLvoid*  data;

	GLuint texture;
	GLuint instanced_count;
};

raster_font_t font_instances[ RASTER_FONT_MAX_FONTS ] = {
	{ /* RASTER_FONT_VINCENT_8X8 */
		.width           = 8,
		.height          = 1024,
		.glyph_count     = 128,
		.glyph_width     = 8,
		.glyph_height    = 8,
		#ifdef USE_PRE_EXPANDED_FONTS
		.bits_per_pixel  = 8,
		#else
		.bits_per_pixel  = 1,
		#endif
		.size            = sizeof(raster_font_vincent),
		.data            = raster_font_vincent,
		.texture         = 0,
		.instanced_count = 0
	},
	{ /* RASTER_FONT_FONT1_8X8 */
		.width           = 8,
		.height          = 1024,
		.glyph_count     = 128,
		.glyph_width     = 8,
		.glyph_height    = 8,
		#ifdef USE_PRE_EXPANDED_FONTS
		.bits_per_pixel  = 8,
		#else
		.bits_per_pixel  = 1,
		#endif
		.size            = sizeof(raster_font_font1),
		.data            = raster_font_font1,
		.texture         = 0,
		.instanced_count = 0
	},
	{ /* RASTER_FONT_FONT2_8X8 */
		.width           = 8,
		.height          = 1024,
		.glyph_count     = 128,
		.glyph_width     = 8,
		.glyph_height    = 8,
		#ifdef USE_PRE_EXPANDED_FONTS
		.bits_per_pixel  = 8,
		#else
		.bits_per_pixel  = 1,
		#endif
		.size            = sizeof(raster_font_font2),
		.data            = raster_font_font2,
		.texture         = 0,
		.instanced_count = 0
	},
	{ /* RASTER_FONT_FONT3_16X16 */
		.width           = 16,
		.height          = 2048,
		.glyph_count     = 128,
		.glyph_width     = 16,
		.glyph_height    = 16,
		#ifdef USE_PRE_EXPANDED_FONTS
		.bits_per_pixel  = 8,
		#else
		.bits_per_pixel  = 1,
		#endif
		.size            = sizeof(raster_font_font3),
		.data            = raster_font_font3,
		.texture         = 0,
		.instanced_count = 0
	},
};

static bool raster_font_initialize( raster_font_t* font );
static void raster_font_deinitialize( raster_font_t* font );

static inline raster_font_t* raster_font( GLuint font )
{
	return &font_instances[ font ];
}

static const GLfloat font_glyph_mesh[] = {
	 0.0f,  0.0f,
	 1.0f,  0.0f,
	 0.0f,  1.0f,
	 0.0f,  1.0f,
	 1.0f,  0.0f,
	 1.0f,  1.0f,
};
#define FONT_GLYPH_MESH_LENGTH         (sizeof(font_glyph_mesh)/sizeof(font_glyph_mesh[0]))


bool raster_font_shader_initialize( raster_font_shader_t* shader )
{
	bool result = false;
	GLuint program = 0;
	GLuint vertex_shader = 0;
	GLuint fragment_shader = 0;
	GLint attribute_vertex = 0;
	GLint uniform_projection = 0;
	GLint uniform_position = 0;
	GLint uniform_size = 0;
	GLint uniform_color = 0;
	GLint uniform_glyph_width = 0;
	GLint uniform_glyph_height = 0;
	GLint uniform_width = 0;
	GLint uniform_height = 0;
	GLint uniform_character = 0;
	GLint uniform_texture = 0;
	GLuint vao = 0;
	GLuint vbo_mesh = 0;

	if( shader->font_count == 0 )
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

		//const char* vertex_shader_source = glsl_shader_load( "./tests/assets/shaders/text.v.glsl" );
		if( !glsl_shader_compile( vertex_shader, vertex_shader_source ) )
		{
			//free( vertex_shader_source );
			goto failure;
		}
		//free( vertex_shader_source );

		fragment_shader = glsl_create( GL_FRAGMENT_SHADER );

		if( !fragment_shader )
		{
			goto failure;
		}

		//const char* fragment_shader_source = glsl_shader_load( "./tests/assets/shaders/text.f.glsl" );
		if( !glsl_shader_compile( fragment_shader, fragment_shader_source ) )
		{
			//free( fragment_shader_source );
			goto failure;
		}
		//free( fragment_shader_source );

		glsl_attach_shader( program, vertex_shader );
		glsl_attach_shader( program, fragment_shader );
		assert(check_gl() == GL_NO_ERROR);

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
		uniform_size         = glsl_bind_uniform( program, "u_size" );
		uniform_glyph_width  = glsl_bind_uniform( program, "u_glyph_width" );
		uniform_glyph_height = glsl_bind_uniform( program, "u_glyph_height" );
		uniform_width        = glsl_bind_uniform( program, "u_width" );
		uniform_height       = glsl_bind_uniform( program, "u_height" );
		uniform_character    = glsl_bind_uniform( program, "u_character" );
		uniform_texture      = glsl_bind_uniform( program, "u_texture" );
		//uniform_width        = glsl_bind_uniform( program, "u_width" );
		//uniform_height       = glsl_bind_uniform( program, "u_height" );
		assert(check_gl() == GL_NO_ERROR);

		glGenVertexArrays( 1, &vao );
		if( !vao ) goto failure;
		glBindVertexArray( vao );

		if( buffer_create( &vbo_mesh, font_glyph_mesh, sizeof(GLfloat), FONT_GLYPH_MESH_LENGTH, GL_ARRAY_BUFFER, GL_STATIC_DRAW ) )
		{
			glEnableVertexAttribArray( attribute_vertex );
			glVertexAttribPointer( attribute_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0 );
			glDisableVertexAttribArray( attribute_vertex );
			assert(check_gl() == GL_NO_ERROR);
		}
		else
		{
			goto failure;
		}

		shader->program              = program;
		shader->attribute_vertex     = attribute_vertex;
		shader->uniform_color        = uniform_color;
		shader->uniform_projection   = uniform_projection;
		shader->uniform_position     = uniform_position;
		shader->uniform_size         = uniform_size;
		shader->uniform_glyph_width  = uniform_glyph_width;
		shader->uniform_glyph_height = uniform_glyph_height;
		shader->uniform_width        = uniform_width;
		shader->uniform_height       = uniform_height;
		shader->uniform_character    = uniform_character;
		shader->uniform_texture      = uniform_texture;
		shader->vao                  = vao;
		shader->vbo_mesh             = vbo_mesh;
	}

	shader->font_count += 1;
	result = true;
	assert(check_gl() == GL_NO_ERROR);


	return result;

failure:
	if( vertex_shader ) glsl_destroy( vertex_shader );
	if( fragment_shader ) glsl_destroy( fragment_shader );
	if( program ) glsl_destroy( program );
	if( vao ) glDeleteVertexArrays( 1, &vao );

	return result;
}

void raster_font_shader_deinitialize( raster_font_shader_t* shader )
{
	if( shader->font_count > 0 )
	{
		if( shader->font_count == 1 )
		{
			glsl_destroy( shader->program );
			glDeleteVertexArrays( 1, &shader->vao );
			glDeleteBuffers( 1, &shader->vbo_mesh );
		}

		shader->font_count -= 1;
	}
}

bool raster_font_initialize( raster_font_t* font )
{
	glGenTextures( 1, &font->texture );

	if( !font->texture )
	{
		return false;
	}

	glBindTexture( GL_TEXTURE_2D, font->texture );

	GLenum pixel_format;// = bit_depth == 32 ? GL_RGBA : GL_RGB;
	switch( font->bits_per_pixel )
	{
		case 1:
		case 8:
			pixel_format = GL_RED;
			break;
		case 32:
			pixel_format = GL_RGBA;
			break;
		case 24: /* fall through */
		default:
			pixel_format = GL_RGB;
			break;

	}


	if( font->bits_per_pixel == 1 )
	{
		GLubyte* pixels = raster_font_expand( font );
		glTexImage2D( GL_TEXTURE_2D, 0, pixel_format, font->width, font->height, 0, pixel_format, GL_UNSIGNED_BYTE, pixels );

		#if 1
		image_t img = {
			.width = font->width,
			.height = font->height,
			.bits_per_pixel = 8,
			.channels = 1,
			.pixels = pixels
		};
		imageio_image_save( &img, "./out.png", IMAGEIO_PNG );
		#endif
		free( pixels );
	}
	else
	{
		glTexImage2D( GL_TEXTURE_2D, 0, pixel_format, font->width, font->height, 0, pixel_format, GL_UNSIGNED_BYTE, font->data );
	}


	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	#if 0
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	#else
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	#endif

	return true;
}

void raster_font_deinitialize( raster_font_t* font )
{
	glDeleteTextures( 1, &font->texture );
	font->texture = 0;
}

GLubyte* raster_font_expand( raster_font_t* font )
{
	size_t pixel_sz = font->glyph_count * font->glyph_width * font->glyph_height;
	GLubyte* pixels = malloc( pixel_sz );

	if( pixels )
	{
		GLubyte* scanlines = (GLubyte*) font->data;

		for( size_t i = 0; i < font->size; i++ )
		{
			GLubyte scanline = scanlines[ i ];
			size_t offset = i << 3;

			pixels[ offset + 0 ] = (scanline & (1 << 7)) ? 0xFF : 0x00;
			pixels[ offset + 1 ] = (scanline & (1 << 6)) ? 0xFF : 0x00;
			pixels[ offset + 2 ] = (scanline & (1 << 5)) ? 0xFF : 0x00;
			pixels[ offset + 3 ] = (scanline & (1 << 4)) ? 0xFF : 0x00;
			pixels[ offset + 4 ] = (scanline & (1 << 3)) ? 0xFF : 0x00;
			pixels[ offset + 5 ] = (scanline & (1 << 2)) ? 0xFF : 0x00;
			pixels[ offset + 6 ] = (scanline & (1 << 1)) ? 0xFF : 0x00;
			pixels[ offset + 7 ] = (scanline & (1 << 0)) ? 0xFF : 0x00;
		}
	}

	return pixels;
}



raster_font_t* raster_font_create( GLushort type )
{
	if( !raster_font_shader_initialize( &font_shader ) )
	{
		goto failed;
	}

	raster_font_t* font = raster_font( type );

	if( font->instanced_count == 0 )
	{
		if( !raster_font_initialize( font ) )
		{
			goto failed;
		}
	}

	font->instanced_count += 1;

	return font;

failed:
	raster_font_shader_deinitialize( &font_shader );
	return NULL;
}

void raster_font_destroy( raster_font_t* fnt )
{
	if( fnt->instanced_count == 1 )
	{
		raster_font_deinitialize( fnt );
	}

	fnt->instanced_count -= 1;
}

GLushort raster_font_glyph_width( const raster_font_t* fnt )
{
	return fnt->glyph_width;
}

GLushort raster_font_glyph_height( const raster_font_t* fnt )
{
	return fnt->glyph_height;
}

void raster_font_write( const raster_font_t* fnt, const vec2_t* position, const vec3_t* color, GLfloat size, const char* text )
{
	assert(check_gl() == GL_NO_ERROR);
	GLint viewport[ 4 ];

	glGetIntegerv( GL_VIEWPORT, viewport );

	mat4_t projection = orthographic( 0, viewport[2], 0, viewport[3], -10.0f, 10.0f );

	glDepthMask( GL_FALSE );

	glUseProgram( font_shader.program );
	assert(check_gl() == GL_NO_ERROR);

	glBindVertexArray( font_shader.vao );

	assert(check_gl() == GL_NO_ERROR);
	glEnableVertexAttribArray( font_shader.attribute_vertex );
	assert(check_gl() == GL_NO_ERROR);

	glUniformMatrix4fv(font_shader.uniform_projection, 1, GL_FALSE, projection.m );
	glUniform3fv( font_shader.uniform_color, 1, (GLfloat*) color );
	assert(check_gl() == GL_NO_ERROR);

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, fnt->texture );
	assert(check_gl() == GL_NO_ERROR);

	glUniform1i( font_shader.uniform_texture, 0 );
	assert(check_gl() == GL_NO_ERROR);

	glUniform1ui( font_shader.uniform_glyph_width, fnt->glyph_width );
	assert(check_gl() == GL_NO_ERROR);
	glUniform1ui( font_shader.uniform_glyph_height, fnt->glyph_height );
	assert(check_gl() == GL_NO_ERROR);
	glUniform1ui( font_shader.uniform_width, fnt->width );
	assert(check_gl() == GL_NO_ERROR);
	glUniform1ui( font_shader.uniform_height, fnt->height );
	assert(check_gl() == GL_NO_ERROR);
	glUniform1f( font_shader.uniform_size, size );
	assert(check_gl() == GL_NO_ERROR);

	for( size_t i = 0; text[i] != '\0' && i < RASTER_FONT_MAX_STRLEN; i++ )
	{
		glUniform3fv( font_shader.uniform_position, 1, (GLfloat*) &VEC3( position->x + i * size * fnt->glyph_width, position->y, 0.0f ) );
		assert(check_gl() == GL_NO_ERROR);
		glUniform1ui( font_shader.uniform_character, text[i] );
		assert(check_gl() == GL_NO_ERROR);
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		assert(check_gl() == GL_NO_ERROR);
	}

	assert(check_gl() == GL_NO_ERROR);

	glDisableVertexAttribArray( font_shader.attribute_vertex );
	assert(check_gl() == GL_NO_ERROR);
	glDepthMask( GL_TRUE );
	assert(check_gl() == GL_NO_ERROR);

	glBindVertexArray( 0 );
}

void raster_font_writef( const raster_font_t* fnt, const vec2_t* position, const vec3_t* color, GLfloat size, const char* format, ... )
{
	char text[ RASTER_FONT_MAX_STRLEN ];
	va_list args;

	va_start( args, format );
	vsnprintf( text, sizeof(text), format, args );
	text[ sizeof(text) - 1 ] = '\0';
	va_end( args );

	raster_font_write( fnt, position, color, size, text );
}

void raster_font_shadowed_writef( const raster_font_t* fnt, const vec2_t* position, const vec3_t* color, const vec3_t* shadow, GLfloat size, const char* format, ... )
{
	char text[ RASTER_FONT_MAX_STRLEN ];
	va_list args;

	va_start( args, format );
	vsnprintf( text, sizeof(text), format, args );
	text[ sizeof(text) - 1 ] = '\0';
	va_end( args );

	raster_font_write( fnt, &VEC2(position->x + 1, position->y - 1), shadow, size, text );
	raster_font_write( fnt, position, color, size, text );
}
