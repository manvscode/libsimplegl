
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "simplegl.h"

GLboolean buffer_create( GLuint* id, const void* geometry, size_t element_size, size_t count, GLenum target /*GL_ARRAY_BUFFER*/, GLenum usage /*GL_STATIC_DRAW*/ )
{
	GLboolean result = GL_FALSE;

	glGenBuffers( 1, id );

	if( *id )
	{
		glBindBuffer( target, *id );
		//GL_ASSERT_NO_ERROR( );
		glBufferData( target, element_size * count, geometry, usage );
		//GL_ASSERT_NO_ERROR( );
		return GL_TRUE;
	}

	return result;
}

GLboolean buffer_destroy( const GLuint* id )
{
	GLboolean result = GL_FALSE;

	if( glIsBuffer(*id) )
	{
		glDeleteBuffers( 1, id );
		//GL_ASSERT_NO_ERROR( );
		return GL_TRUE;
	}

	return result;
}
