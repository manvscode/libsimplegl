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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "simplegl.h"

GLboolean buffer_create( GLuint* id, const GLvoid* geometry, size_t element_size, size_t count, GLenum target /*GL_ARRAY_BUFFER*/, GLenum usage /*GL_STATIC_DRAW*/ )
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
