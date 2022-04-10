# Add new files in alphabetical order. Thanks.
libsimplegl_src = \
                  3d-axes.c \
                  buffers.c \
                  camera.c \
                  cube-map.c \
                  extensions.c \
                  grid.c \
                  objects.c \
                  overlays.c \
                  projections.c \
                  raster-font-font1.c \
                  raster-font-font2.c \
                  raster-font-font3.c \
                  raster-font-vincent.c \
                  raster-font.c \
                  shaders.c \
                  simplegl.c \
                  textures.c

# Add new files in alphabetical order. Thanks.
libsimplegl_headers = simplegl.h

library_includedir      = $(includedir)/@PACKAGE_NAME@/
library_include_HEADERS = $(libsimplegl_headers)



# Library
lib_LTLIBRARIES                            = $(top_builddir)/lib/libsimplegl.la
__top_builddir__lib_libsimplegl_la_SOURCES = $(libsimplegl_src)
__top_builddir__lib_libsimplegl_la_CFLAGS  = $(M3D_CFLAGS) $(IMAGEIO_CFLAGS) $(GL_CFLAGS) $(X11_CFLAGS)
__top_builddir__lib_libsimplegl_la_LIBADD  = -lm $(M3D_LIBS) $(IMAGEIO_LIBS) $(GL_LIBS) $(X11_LIBS)
#-L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -framework OpenGL


