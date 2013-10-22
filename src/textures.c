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
	glGenTextures( 1, p_texture );
}

GLuint tex2d_create( void )
{
	GLuint texture = 0;
	glGenTextures( 1, &texture );
	assert( texture > 0 );
	assert(check_gl() == GL_NO_ERROR);
	return texture;
}

void tex2d_destroy( GLuint texture )
{
	glDeleteTextures( 1, &texture );
}

bool tex2d_load( GLuint texture, const char* filename, GLint min_filter, GLint mag_filter, GLubyte flags )
{
	bool result = false;
	const char* extension = strrchr( filename, '.' );
	image_file_format_t format = IMAGEIO_PNG;
	image_t image;
	assert(check_gl() == GL_NO_ERROR);

	if( extension )
	{
		extension += 1;

		if( strcasecmp( "png", extension ) == 0 )
		{
			format = IMAGEIO_PNG;
			#ifdef SIMPLEGL_DEBUG
			printf( "Loading PNG: %s\n", filename );
			#endif
		}
		else if( strcasecmp( "bmp", extension ) == 0 )
		{
			format = IMAGEIO_BMP;
			#ifdef SIMPLEGL_DEBUG
			printf( "Loading BMP: %s\n", filename );
			#endif
		}
		else if( strcasecmp( "tga", extension ) == 0 )
		{
			format = IMAGEIO_TGA;
			#ifdef SIMPLEGL_DEBUG
			printf( "Loading TGA: %s\n", filename );
			#endif
		}
		else if( strcasecmp( "pvr", extension ) == 0 || strcasecmp( "pvrtc", extension ) == 0 )
		{
			format = IMAGEIO_PVR;
			flags |= TEX2D_COMPRESS;
			#ifdef SIMPLEGL_DEBUG
			printf( "Loading PVRTC: %s\n", filename );
			#endif
		}
	}

	if( imageio_image_load( &image, filename, format ) )
	{
		#if TARGET_OS_IPHONE
		if( !is_power_of_2(image.width) || !is_power_of_2(image.height) )
		{
			/* iOS devices require texture dimensions to be
			 * a power of 2.
			 */
			imageio_image_destroy( &image );
			goto failure;
		}
		#endif

		if( format == IMAGEIO_PNG )
		{
			/* These pesky PNG files need to be flipped vertically to be
			 * correctly oriented for OpenGL.
			 */
			imageio_flip_vertically( image.width, image.height, image.bits_per_pixel >> 3, image.pixels );
		}

		assert(check_gl() == GL_NO_ERROR);

		tex2d_setup_texture( texture, image.width, image.height, image.bits_per_pixel, image.pixels, min_filter, mag_filter, flags );
		assert( glIsTexture(texture) );

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

failure:
	return result;
}

bool tex2d_load_for_2d( GLuint texture, const char* filename )
{
	return tex2d_load( texture, filename, GL_LINEAR, GL_LINEAR, TEX2D_CLAMP_S | TEX2D_CLAMP_T );
}

bool tex2d_load_for_3d( GLuint texture, const char* filename, GLubyte flags )
{
	return tex2d_load( texture, filename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, flags );
}


void tex2d_setup_texture( GLuint texture, GLsizei width, GLsizei height, GLbyte bit_depth, const GLvoid* pixels, GLint min_filter, GLint mag_filter, GLubyte flags )
{
	glBindTexture( GL_TEXTURE_2D, texture );

	#if 0
	GLint mipmap_count = 1;
	if( mipmap_count <= 0 )
	{
		mipmap_count = 1;
	}
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmap_count - 1 );
	#endif

	if( flags & TEX2D_COMPRESS )
	{
		#if TARGET_OS_IPHONE
		// TODO: Figure out how to choose the right format.
		GLenum pixel_format = (bit_depth == 4 ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
		GLsizei image_size = width * height / 2;
		#else
		GLenum pixel_format = (bit_depth == 32 ? GL_COMPRESSED_RGBA : GL_COMPRESSED_RGB);
		#endif

		glCompressedTexImage2D( GL_TEXTURE_2D, 0, pixel_format, width, height, 0 /*must be zero*/, image_size, pixels );
		assert(check_gl() == GL_NO_ERROR);
	}
	else
	{
		GLenum pixel_format = (bit_depth == 32 ? GL_RGBA : GL_RGB);
		GLint border = (flags & TEX2D_BORDER) ? 1 : 0;
		glTexImage2D( GL_TEXTURE_2D, 0, pixel_format, width, height, border, pixel_format, GL_UNSIGNED_BYTE, pixels );
		assert(check_gl() == GL_NO_ERROR);
	}

	bool generate_mipmaps = false;

	switch( min_filter )
	{
		case GL_NEAREST_MIPMAP_LINEAR:
		case GL_NEAREST_MIPMAP_NEAREST:
		case GL_LINEAR_MIPMAP_LINEAR:
		case GL_LINEAR_MIPMAP_NEAREST:
			generate_mipmaps = true;
			break;
		default:
			break;
	}

	/* Only GL_LINEAR and GL_NEAREST are valid magnification filters */
	switch( mag_filter )
	{
		case GL_NEAREST_MIPMAP_LINEAR:
		case GL_NEAREST_MIPMAP_NEAREST:
			mag_filter = GL_NEAREST;
			break;
		case GL_LINEAR_MIPMAP_LINEAR:
		case GL_LINEAR_MIPMAP_NEAREST:
			mag_filter = GL_LINEAR;
			break;
		default:
			break;
	}

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TEX2D_CLAMP_S) ? GL_CLAMP_TO_EDGE : GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TEX2D_CLAMP_T) ? GL_CLAMP_TO_EDGE : GL_REPEAT );

	if( generate_mipmaps )
	{
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	assert(check_gl() == GL_NO_ERROR);
}
