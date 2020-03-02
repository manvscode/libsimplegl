if ENABLE_TESTS
#AM_CFLAGS = -std=c99 -pg -g -ggdb -O0 -I$(top_builddir)/src/ -I. -I.. -I/usr/local/include/ -L$(top_builddir)/lib/  -L/usr/local/lib
#AM_CXXFLAGS = -std=c++0x -pg -g -ggdb -O0 -I$(top_builddir)/src/ -I. -I.. -I/usr/local/include/ -L$(top_builddir)/lib/ -L/usr/local/lib
AM_CFLAGS  = -D_THREAD_SAFE -DHAVE_OPENGL $(M3D_CFLAGS) $(GL_CFLAGS) $(SDL2_CFLAGS)
AM_LDFLAGS = -static
LDADD      = -lm $(M3D_LIBS) $(GL_LIBS) $(SDL2_LIBS) $(top_builddir)/lib/libsimplegl.la

bin_PROGRAMS = \
$(top_builddir)/bin/triangle \
$(top_builddir)/bin/cube \
$(top_builddir)/bin/volume-render \
$(top_builddir)/bin/3daxes \
$(top_builddir)/bin/skybox \
$(top_builddir)/bin/flag
#$(top_builddir)/bin/fire



__top_builddir__bin_triangle_SOURCES = triangle.c
#__top_builddir__bin_triangle_CFLAGS  =

__top_builddir__bin_cube_SOURCES = cube.c
#__top_builddir__bin_cube_CFLAGS  =

__top_builddir__bin_volume_render_SOURCES = volume-render.c
#__top_builddir__bin_volume_render_CFLAGS  =

__top_builddir__bin_3daxes_SOURCES = 3daxes.c
#__top_builddir__bin_3daxes_CFLAGS  =

__top_builddir__bin_skybox_SOURCES = skybox.c
#__top_builddir__bin_skybox_CFLAGS  =

__top_builddir__bin_flag_SOURCES = flag.c
#__top_builddir__bin_flag_CFLAGS  =

#__top_builddir__bin_fire_SOURCES = fire.c
#__top_builddir__bin_fire_CFLAGS  = -D_THREAD_SAFE -DHAVE_OPENGL $(GL_CFLAGS)
endif
