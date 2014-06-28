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




void tex_prepare_image( image_file_format_t format, image_t* image )
{
    if( format == IMAGEIO_PVR )
    {

    }
    else if( format == IMAGEIO_PNG )
    {
        /* These pesky PNG files need to be flipped vertically to be
         * correctly oriented for OpenGL.
         */
        imageio_flip_horizontally( image->width, image->height, image->bits_per_pixel >> 3, image->pixels );
        //imageio_flip_vertically( image->width, image->height, image->bits_per_pixel >> 3, image->pixels );
    }
}


GLuint tex_create( void )
{
	GLuint texture = 0;
	glGenTextures( 1, &texture );
	assert( texture > 0 );
	assert(check_gl() == GL_NO_ERROR);
	return texture;
}

void tex_destroy( GLuint texture )
{
	glDeleteTextures( 1, &texture );
}

void tex_setup_texture( GLuint texture, GLsizei width, GLsizei height, GLsizei depth, GLbyte bit_depth, const GLvoid* pixels, GLint min_filter, GLint mag_filter, GLubyte flags, GLuint texture_dimensions )
{
	GLuint texture_type;

	switch( texture_dimensions )
	{
		case 3:
			texture_type = GL_TEXTURE_3D;
			break;
		case 2:
			texture_type = GL_TEXTURE_2D;
			break;
		case 1: /* fall-through */
		default:
			texture_type = GL_TEXTURE_1D;
			break;
	}

	glBindTexture( texture_type, texture );

	#if 0
	GLint mipmap_count = 1;
	if( mipmap_count <= 0 )
	{
		mipmap_count = 1;
	}
	glTexParameteri( texture_type, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( texture_type, GL_TEXTURE_MAX_LEVEL, mipmap_count - 1 );
	#endif

	if( texture_dimensions == 1 )
	{
		GLenum pixel_format;
		switch( bit_depth )
		{
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
		const GLint border = flags & TEX_BORDER;
		glTexImage1D( texture_type, 0, pixel_format, width, border, pixel_format, GL_UNSIGNED_BYTE, pixels );
		assert(check_gl() == GL_NO_ERROR);
	}
	else if( texture_dimensions == 2 )
	{
		if( (flags & TEX_COMPRESS_RGB) || (flags & TEX_COMPRESS_RGBA) )
		{
			#if TARGET_OS_IPHONE
			GLenum pixel_format;
			// TODO: Figure out how to choose the right format.
			switch( bit_depth )
			{
				case 2:
					pixel_format = (flags & TEX_COMPRESS_RGBA) ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
					break;
				case 4: /* fall through */
				default:
					pixel_format = (flags & TEX_COMPRESS_RGBA) ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
					break;

			}
			GLsizei image_size = width * height / 2;
			glCompressedTexImage2D( texture_type, 0, pixel_format, width, height, 0 /*must be zero*/, image_size, pixels );
			assert(check_gl() == GL_NO_ERROR);
			#else
			GLenum pixel_format = (bit_depth == 32 ? GL_COMPRESSED_RGBA : GL_COMPRESSED_RGB);
			GLsizei image_size = width * height / 2;
			glCompressedTexImage2D( texture_type, 0, pixel_format, width, height, 0 /*must be zero*/, image_size, pixels );
			assert(check_gl() == GL_NO_ERROR);
			assert( false && "Not implemented" );
			#endif
		}
		else
		{
			GLenum pixel_format;// = bit_depth == 32 ? GL_RGBA : GL_RGB;
			switch( bit_depth )
			{
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
			const GLint border = flags & TEX_BORDER;
			glTexImage2D( texture_type, 0, pixel_format, width, height, border, pixel_format, GL_UNSIGNED_BYTE, pixels );
			assert(check_gl() == GL_NO_ERROR);
		}
	}
	else if( texture_dimensions == 3 ) /* 3D textures */
	{
		GLenum pixel_format;
		GLenum internal_format;
		GLenum type;

		switch( bit_depth )
		{
			case 32:
				internal_format = GL_RGBA;
				pixel_format    = GL_RGBA;
				type            = GL_UNSIGNED_BYTE;
				break;
			case 24:
				internal_format = GL_RGB;
				pixel_format    = GL_RGB;
				type            = GL_UNSIGNED_BYTE;
				break;
			case 16:
				internal_format = GL_RED;
				pixel_format    = GL_RED;
				type            = GL_UNSIGNED_SHORT;
				break;
			case 8: /* fall through */
			default:
				internal_format = GL_RED;
				pixel_format    = GL_RED;
				type            = GL_UNSIGNED_BYTE;
				break;

		}
		const GLint border = 0; /* Must be zero for 3D textures */
		glTexImage3D( texture_type, 0, internal_format, width, height, depth, border, pixel_format, type, pixels );
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

	glTexParameteri( texture_type, GL_TEXTURE_MIN_FILTER, min_filter );
	glTexParameteri( texture_type, GL_TEXTURE_MAG_FILTER, mag_filter );
	glTexParameteri( texture_type, GL_TEXTURE_WRAP_S, (flags & TEX_CLAMP_S) ? GL_CLAMP_TO_EDGE : GL_REPEAT );

	if( texture_dimensions >= 2 )
	{
		glTexParameteri( texture_type, GL_TEXTURE_WRAP_T, (flags & TEX_CLAMP_T) ? GL_CLAMP_TO_EDGE : GL_REPEAT );
	}

	if( texture_dimensions >= 3 )
	{
		glTexParameteri( texture_type, GL_TEXTURE_WRAP_R, (flags & TEX_CLAMP_R) ? GL_CLAMP_TO_EDGE : GL_REPEAT );
	}

	if( generate_mipmaps )
	{
		glGenerateMipmap( texture_type );
	}

	assert(check_gl() == GL_NO_ERROR);
}


bool tex_load_1d( GLuint texture, const GLchar* filename, GLint min_filter, GLint mag_filter, GLubyte flags )
{
	bool result = false;

	if( !filename || *filename == '\0' )
	{
		goto failure;
	}

	image_file_format_t format;
	image_t image;
	assert(check_gl() == GL_NO_ERROR);

    #ifdef SIMPLEGL_DEBUG
    printf( "Loading %s\n", filename );
    #endif

	if( imageio_load( &image, filename, &format ) )
	{
		if( format == IMAGEIO_PVR )
		{
			flags |= (image.channels == 4 ? TEX_COMPRESS_RGBA : TEX_COMPRESS_RGB);
		}
		else if( format == IMAGEIO_PNG )
		{
			/* These pesky PNG files need to be flipped vertically to be
			 * correctly oriented for OpenGL.
			 */
			imageio_flip_vertically( image.width, image.height, image.bits_per_pixel >> 3, image.pixels );
		}

		assert(check_gl() == GL_NO_ERROR);

		tex_setup_texture( texture, image.width, image.height, 0, image.bits_per_pixel, image.pixels, min_filter, mag_filter, flags, 1 );
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

bool tex_load_2d( GLuint texture, const GLchar* filename, GLint min_filter, GLint mag_filter, GLubyte flags )
{
	bool result = false;

	if( !filename || *filename == '\0' )
	{
		goto failure;
	}

	image_file_format_t format;
	image_t image;
	assert(check_gl() == GL_NO_ERROR);

    #ifdef SIMPLEGL_DEBUG
    printf( "Loading %s\n", filename );
    #endif

	if( imageio_load( &image, filename, &format ) )
	{
		if( format == IMAGEIO_PVR )
		{
			flags |= (image.channels == 4 ? TEX_COMPRESS_RGBA : TEX_COMPRESS_RGB);
		}
		else if( format == IMAGEIO_PNG )
		{
			/* These pesky PNG files need to be flipped vertically to be
			 * correctly oriented for OpenGL.
			 */
			imageio_flip_vertically( image.width, image.height, image.bits_per_pixel >> 3, image.pixels );
		}

		assert(check_gl() == GL_NO_ERROR);

		tex_setup_texture( texture, image.width, image.height, 0, image.bits_per_pixel, image.pixels, min_filter, mag_filter, flags, 2 );
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

bool tex_load_2d_with_linear( GLuint texture, const GLchar* filename, GLubyte flags )
{
	return tex_load_2d( texture, filename, GL_LINEAR, GL_LINEAR, flags );
}

bool tex_load_2d_with_mipmaps( GLuint texture, const GLchar* filename, GLubyte flags )
{
	return tex_load_2d( texture, filename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, flags );
}

bool tex_load_3d( GLuint texture, const GLchar* filename, GLsizei voxel_bit_depth, GLsizei width, GLsizei height, GLsizei length, GLint min_filter, GLint mag_filter, GLubyte flags )
{
	bool result = false;

	if( !filename || *filename == '\0' )
	{
		goto failure;
	}

	const GLchar* extension = strrchr( filename, '.' );
	assert(check_gl() == GL_NO_ERROR);

	if( extension )
	{
		extension += 1;

		if( strcasecmp( "raw", extension ) == 0 )
		{
			#ifdef SIMPLEGL_DEBUG
			printf( "Loading RAW: %s (%dx%dx%d,voxel_depth=%d)\n", filename, width, height, length, voxel_bit_depth );
			#endif
		}
		else
		{
			goto failure;
		}
	}

	/* load raw data */
	{
		FILE* volume_data_file = fopen( filename, "r" );
		if( volume_data_file )
		{
			size_t voxel_size;
			switch( voxel_bit_depth )
			{
				case 32:
					voxel_size = sizeof(uint32_t);
					break;
				case 24:
					voxel_size = sizeof(uint8_t) * 3;
					break;
				case 16:
					voxel_size = sizeof(uint16_t);
					break;
				case 8: /* fall-through */
					voxel_size = sizeof(uint8_t);
					break;
				default:
					goto failure;
					break;
			}

			const size_t volume_data_count = width * height * length;
			void* volume_data = malloc( voxel_size * volume_data_count );
			size_t objs_read = fread( volume_data, voxel_size * volume_data_count, 1, volume_data_file );

			if( objs_read == 1 )
			{
				fclose( volume_data_file );

				tex_setup_texture( texture, width, height, length, voxel_bit_depth, volume_data, min_filter, mag_filter, flags, 3 );
				assert(check_gl() == GL_NO_ERROR);
				assert( glIsTexture(texture) );
				free( volume_data );
				result = true;
			}
			else
			{
				#ifdef SIMPLEGL_DEBUG
				printf( "Read %zu objects. Failed reading data from %s.\n", objs_read, filename );
				assert( objs_read == 1 );
				#endif
			}
		}
	}

failure:
	return result;
}

