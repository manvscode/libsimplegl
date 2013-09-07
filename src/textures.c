#include <libimageio/imageio.h>
#include <string.h>
#include <assert.h>
#include "simplegl.h"


void _tex2d_create( GLuint* p_texture )
{
	assert( p_texture );
	glEnable( GL_TEXTURE_2D );
	glGenTextures( 1, p_texture );
}

GLuint tex2d_create( void )
{
	GLuint texture = 0;
	glEnable( GL_TEXTURE_2D );
	glGenTextures( 1, &texture );
	assert( texture > 0 );
	return texture;
}

void tex2d_destroy( GLuint texture )
{
	glDeleteTextures( 1, &texture );
}

bool tex2d_load( GLuint texture, const char* filename, GLint min_filter, GLint mag_filter, bool clamp )
{
	bool result = false;
	const char* extension = strchr( filename, '.' );
	image_file_format_t format = PNG;
	image_t image;

	if( extension )
	{
		extension += 1;

		if( strcasecmp( "png", extension ) == 0 )
		{
			format = PNG;
		}
		else if( strcasecmp( "bmp", extension ) == 0 )
		{
			format = BMP;
		}
		else if( strcasecmp( "tga", extension ) == 0 )
		{
			format = TGA;
		}
	}

	//glPushAttrib( GL_ENABLE_BIT );

	if( imageio_image_load( &image, filename, format ) )
	{
		GLenum pixel_format = image.bits_per_pixel == 32 ? GL_RGBA : GL_RGB;

		glEnable( GL_TEXTURE_2D );

		glBindTexture( GL_TEXTURE_2D, texture );
		glTexImage2D( GL_TEXTURE_2D, 0, pixel_format, image.width, image.height, 0, pixel_format, GL_UNSIGNED_BYTE, image.pixels );


		//#if SIMPLEGL_2D
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//#else
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		//#endif

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter > 0 ? min_filter : GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter > 0 ? mag_filter : GL_LINEAR );

		if( clamp )
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		}
		else
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}


		/* Dispose of image */
		imageio_image_destroy( &image );
		result = true;
	}

	//glPopAttrib( );
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

