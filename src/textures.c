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
#include <libimageio/imageio.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "simplegl.h"


void _tex2d_create( GLuint* p_texture )
{
	assert( p_texture );
	//glEnable( GL_TEXTURE_2D );
	GL_ASSERT_NO_ERROR( );
	glGenTextures( 1, p_texture );
	GL_ASSERT_NO_ERROR( );
}

GLuint tex2d_create( void )
{
	GLuint texture = 0;
	//glEnable( GL_TEXTURE_2D );
	GL_ASSERT_NO_ERROR( );
	glGenTextures( 1, &texture );
	GL_ASSERT_NO_ERROR( );
	assert( texture > 0 );
	return texture;
}

void tex2d_destroy( GLuint texture )
{
	glDeleteTextures( 1, &texture );
	GL_ASSERT_NO_ERROR( );
}

bool tex2d_load( GLuint texture, const char* filename, GLint min_filter, GLint mag_filter, bool clamp )
{
	bool result = false;
	const char* extension = strrchr( filename, '.' );
	image_file_format_t format = IMAGEIO_PNG;
	image_t image;

	if( extension )
	{
		extension += 1;

		if( strcasecmp( "png", extension ) == 0 )
		{
			format = IMAGEIO_PNG;
			#ifdef SIMPLEGL_DEBUG
			printf( "Loading TGA: %s\n", filename );
			#endif
		}
		else if( strcasecmp( "bmp", extension ) == 0 )
		{
			format = IMAGEIO_BMP;
			#ifdef SIMPLEGL_DEBUG
			printf( "Loading TGA: %s\n", filename );
			#endif
		}
		else if( strcasecmp( "tga", extension ) == 0 )
		{
			format = IMAGEIO_TGA;
			#ifdef SIMPLEGL_DEBUG
			printf( "Loading TGA: %s\n", filename );
			#endif
		}
	}

	if( imageio_image_load( &image, filename, format ) )
	{
		if( format == IMAGEIO_PNG )
		{
			/* These pesky PNG files need to be flipped vertically to be
			 * correctly oriented for OpenGL.
			 */
			imageio_flip_vertically( image.width, image.height, image.bits_per_pixel >> 3, image.pixels );
		}


		tex2d_setup_texture( texture, image.width, image.height, image.bits_per_pixel, image.pixels, min_filter, mag_filter, clamp );

		// Dispose of image
		imageio_image_destroy( &image );
		result = true;
	}
	#ifdef SIMPLEGL_DEBUG
	else
	{
		fprintf( stderr, "Failed to load: %s\n", filename );
	}
	#endif

	return result;
}

bool tex2d_load_for_2d( GLuint texture, const char* filename )
{
	return tex2d_load( texture, filename, GL_NEAREST, GL_NEAREST, true );
}

bool tex2d_load_for_3d( GLuint texture, const char* filename, bool clamp )
{
	return tex2d_load( texture, filename, GL_LINEAR, GL_LINEAR, clamp );
}

void tex2d_setup_texture( GLuint texture, GLsizei width, GLsizei height, GLbyte bit_depth, const void* pixels, GLint min_filter, GLint mag_filter, bool clamp )
{
	//glPushAttrib( GL_ENABLE_BIT );

		GLenum pixel_format = bit_depth == 32 ? GL_RGBA : GL_RGB;

		//glEnable( GL_TEXTURE_2D );

		glBindTexture( GL_TEXTURE_2D, texture );
		GL_ASSERT_NO_ERROR( );
		glTexImage2D( GL_TEXTURE_2D, 0, pixel_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, pixels );
		GL_ASSERT_NO_ERROR( );


		//#if SIMPLEGL_2D
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//#else
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		//#endif

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter > 0 ? min_filter : GL_LINEAR );
		GL_ASSERT_NO_ERROR( );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter > 0 ? mag_filter : GL_LINEAR );
		GL_ASSERT_NO_ERROR( );

		if( clamp )
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			GL_ASSERT_NO_ERROR( );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			GL_ASSERT_NO_ERROR( );
		}
		else
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			GL_ASSERT_NO_ERROR( );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			GL_ASSERT_NO_ERROR( );
		}


	//glPopAttrib( );
}
