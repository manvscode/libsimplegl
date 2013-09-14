# Add new files in alphabetical order. Thanks.
libsimplegl_src = vec2.c vec3.c vec4.c mat2.c mat3.c mat4.c projections.c shaders.c textures.c transformations.c simplegl.c objects.c

# Add new files in alphabetical order. Thanks.
libsimplegl_headers = vec2.h vec3.h vec4.h mat2.h mat3.h mat4.h mathematics.h simplegl.h

library_includedir      = $(includedir)/libsimplegl/
library_include_HEADERS = $(libsimplegl_headers)

# Library
lib_LTLIBRARIES                            = $(top_builddir)/lib/libsimplegl.la
__top_builddir__lib_libsimplegl_la_SOURCES = $(libsimplegl_src)
__top_builddir__lib_libsimplegl_la_LDFLAGS = -lGL -lGLU -lm -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -framework OpenGL
