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
#include <libimageio/imageio.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "simplegl.h"


extern void tex_prepare_image( image_file_format_t format, image_t* image );


bool tex_cube_map_setup( GLuint texture, const char* xpos, const char* xneg, const char* ypos, const char* yneg, const char* zpos, const char* zneg )
{
	bool result = false;
	image_t xpos_image = { .pixels = NULL };
	image_t xneg_image = { .pixels = NULL };
	image_t ypos_image = { .pixels = NULL };
	image_t yneg_image = { .pixels = NULL };
	image_t zpos_image = { .pixels = NULL };
	image_t zneg_image = { .pixels = NULL };
    image_file_format_t format;

	if( xpos )
	{
		if( imageio_load( &xpos_image, xpos, &format ) )
		{
			tex_prepare_image( format, &xpos_image );
		}
		else
		{
			#ifdef SIMPLEGL_DEBUG
			fprintf( stderr, "Failed to load: %s\n", xpos );
			#endif
			goto done;
		}
	}

	if( xneg )
	{
		if( imageio_load( &xneg_image, xneg, &format ) )
		{
			tex_prepare_image( format, &xneg_image );
		}
		else
		{
			#ifdef SIMPLEGL_DEBUG
			fprintf( stderr, "Failed to load: %s\n", xneg );
			#endif
			goto done;
		}
	}

	if( ypos )
	{
		if( imageio_load( &ypos_image, ypos, &format ) )
		{
			tex_prepare_image( format, &ypos_image );
		}
		else
		{
			#ifdef SIMPLEGL_DEBUG
			fprintf( stderr, "Failed to load: %s\n", ypos );
			#endif
			goto done;
		}
	}

	if( yneg )
	{
		if( imageio_load( &yneg_image, yneg, &format ) )
		{
			tex_prepare_image( format, &yneg_image );
		}
		else
		{
			#ifdef SIMPLEGL_DEBUG
			fprintf( stderr, "Failed to load: %s\n", yneg );
			#endif
			goto done;
		}
	}

	if( zpos )
	{
		if( imageio_load( &zpos_image, zpos, &format ) )
		{
			tex_prepare_image( format, &zpos_image );
		}
		else
		{
			#ifdef SIMPLEGL_DEBUG
			fprintf( stderr, "Failed to load: %s\n", zpos );
			#endif
			goto done;
		}
	}

	if( zneg )
	{
		if( imageio_load( &zneg_image, zneg, &format ) )
		{
			tex_prepare_image( format, &zneg_image );
		}
		else
		{
			#ifdef SIMPLEGL_DEBUG
			fprintf( stderr, "Failed to load: %s\n", zneg );
			#endif
			goto done;
		}
	}

	assert(check_gl() == GL_NO_ERROR);
    glBindTexture( GL_TEXTURE_CUBE_MAP, texture );
	assert(check_gl() == GL_NO_ERROR);
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	assert(check_gl() == GL_NO_ERROR);

    if( xpos_image.pixels )
	{
		#ifdef SIMPLEGL_DEBUG
		fprintf( stdout, "+X: %s\n", imageio_image_string( &xpos_image ) );
		#endif
		GLenum pixel_format = (xpos_image.bits_per_pixel == 32 ? GL_RGBA : GL_RGB);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, pixel_format, xpos_image.width, xpos_image.height, 0, pixel_format, GL_UNSIGNED_BYTE, xpos_image.pixels );
	}

    if( xneg_image.pixels )
	{
		#ifdef SIMPLEGL_DEBUG
		fprintf( stdout, "-X: %s\n", imageio_image_string( &xneg_image ) );
		#endif
		GLenum pixel_format = (xneg_image.bits_per_pixel == 32 ? GL_RGBA : GL_RGB);
    	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, pixel_format, xneg_image.width, xneg_image.height, 0, pixel_format, GL_UNSIGNED_BYTE, xneg_image.pixels );
	}

    if( ypos_image.pixels )
	{
		#ifdef SIMPLEGL_DEBUG
		fprintf( stdout, "+Y: %s\n", imageio_image_string( &ypos_image ) );
		#endif
		GLenum pixel_format = (ypos_image.bits_per_pixel == 32 ? GL_RGBA : GL_RGB);
    	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, pixel_format, ypos_image.width, ypos_image.height, 0, pixel_format, GL_UNSIGNED_BYTE, ypos_image.pixels );
	}

    if( yneg_image.pixels )
	{
		#ifdef SIMPLEGL_DEBUG
		fprintf( stdout, "-Y: %s\n", imageio_image_string( &yneg_image ) );
		#endif
		GLenum pixel_format = (yneg_image.bits_per_pixel == 32 ? GL_RGBA : GL_RGB);
    	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, pixel_format, yneg_image.width, yneg_image.height, 0, pixel_format, GL_UNSIGNED_BYTE, yneg_image.pixels );
	}

    if( zpos_image.pixels )
	{
		#ifdef SIMPLEGL_DEBUG
		fprintf( stdout, "+Z: %s\n", imageio_image_string( &zpos_image ) );
		#endif
		GLenum pixel_format = (zpos_image.bits_per_pixel == 32 ? GL_RGBA : GL_RGB);
    	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, pixel_format, zpos_image.width, zpos_image.height, 0, pixel_format, GL_UNSIGNED_BYTE, zpos_image.pixels );
	}

    if( zneg_image.pixels )
	{
		#ifdef SIMPLEGL_DEBUG
		fprintf( stdout, "-Z: %s\n", imageio_image_string( &zneg_image ) );
		#endif
		GLenum pixel_format = (zneg_image.bits_per_pixel == 32 ? GL_RGBA : GL_RGB);
    	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, pixel_format, zneg_image.width, zneg_image.height, 0, pixel_format, GL_UNSIGNED_BYTE, zneg_image.pixels );
	}

	assert(check_gl() == GL_NO_ERROR);
	result = true;

done:
	if( xpos_image.pixels ) imageio_image_destroy( &xpos_image );
	if( xneg_image.pixels ) imageio_image_destroy( &xneg_image );
	if( ypos_image.pixels ) imageio_image_destroy( &ypos_image );
	if( yneg_image.pixels ) imageio_image_destroy( &yneg_image );
	if( zpos_image.pixels ) imageio_image_destroy( &zpos_image );
	if( zneg_image.pixels ) imageio_image_destroy( &zneg_image );

	return result;
}

