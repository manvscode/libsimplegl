# Add new files in alphabetical order. Thanks.
libsimplegl_src = 3d-axes.c buffers.c camera.c cube-map.c projections.c shaders.c textures.c transformations.c simplegl.c objects.c raster-font.c default-font.c

# Add new files in alphabetical order. Thanks.
libsimplegl_headers = simplegl.h

library_includedir      = $(includedir)/libsimplegl/
library_include_HEADERS = $(libsimplegl_headers)

# Library
lib_LTLIBRARIES                            = $(top_builddir)/lib/libsimplegl.la
__top_builddir__lib_libsimplegl_la_SOURCES = $(libsimplegl_src)
__top_builddir__lib_libsimplegl_la_LDFLAGS = -l3dmath -lGL -lGLU -lm -lpng -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -framework OpenGL
