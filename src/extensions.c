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
#include <stdlib.h>
#include <string.h>
#include "simplegl.h"
#ifdef __linux__
//# include <X11/X.h>
//# include <X11/Xlib.h>
# include <GL/glx.h>
# include <GL/glxext.h>
#endif


#if defined(__APPLE__)
#include <AvailabilityMacros.h>

#ifdef MAC_OS_X_VERSION_10_3

#include <dlfcn.h>

void* appleGetProcAddress(const GLchar* name)
{
	static void* image = NULL;
	void* addr;
	if (NULL == image)
	{
		image = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
	}
	if( !image ) return NULL;
	addr = dlsym( image, (const char*)name );
	if( addr ) return addr;
	//return dlGetProcAddress( name ); // try next for glx symbols
	return NULL;
}
#else

#include <mach-o/dyld.h>
void * appleGetProcAddress( const GLubyte* name )
{
	static const struct mach_header* image = NULL;

	if( !image )
	{
		image = NSAddImage( "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR );
	}

    NSSymbol symbol = NULL;
    char *symbolName = malloc( strlen( name ) + 2 );

	if( symbolName )
	{
		strcpy( symbolName + 1, name );
		symbolName[0] = '_';

		if( image )
		{
  			symbol = NSLookupSymbolInImage( image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR );
		}

		free( symbolName );
	}

    return symbol ? NSAddressOfSymbol( symbol ) : NULL;
}
#endif
#endif

bool gl_has_extension( const GLchar* ext )
{
	bool found = false;
	#ifdef GL_NUM_EXTENSIONS
	int extensionsCount = 0;
	glGetIntegerv( GL_NUM_EXTENSIONS, &extensionsCount );

	for( int i = 0; !found && i < extensionsCount; i++ )
	{
		const GLchar* extension = (const GLchar*) glGetStringi( GL_EXTENSIONS, i );
		found = extension && (strcmp( extension, ext ) == 0);
	}
	#endif

	return found;
}

void* gl_extension( const GLchar* procName )
{
	#if _WIN64 || _WIN32
	return wglGetProcAddress( procName );
	#elif __APPLE__
	return appleGetProcAddress( procName );
	#else
	return glXGetProcAddressARB( (const GLubyte*) procName );
	#endif
}
