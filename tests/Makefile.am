if ENABLE_TESTS
AM_CFLAGS = -std=c99 -pg -g -ggdb -O0 -I$(top_builddir)/src/ -I. -I.. -I/usr/local/include/ -L$(top_builddir)/lib/ 
AM_CXXFLAGS = -std=c++0x -pg -g -ggdb -O0 -I$(top_builddir)/src/ -I. -I.. -I/usr/local/include/ -L$(top_builddir)/lib/
LDADD = -limageio #-lGL

bin_PROGRAMS = \
$(top_builddir)/bin/cube \
$(top_builddir)/bin/3daxes \
$(top_builddir)/bin/fire

__top_builddir__bin_cube_SOURCES = cube.c
__top_builddir__bin_cube_CFLAGS  = -D_THREAD_SAFE -DHAVE_OPENGL
__top_builddir__bin_cube_LDFLAGS = -lm -l3dmath -lSDL2 -framework OpenGL $(top_builddir)/lib/.libs/libsimplegl.a

__top_builddir__bin_3daxes_SOURCES = 3daxes.c
__top_builddir__bin_3daxes_CFLAGS  = -D_THREAD_SAFE -DHAVE_OPENGL
__top_builddir__bin_3daxes_LDFLAGS = -lm -l3dmath -lSDL2 -framework OpenGL $(top_builddir)/lib/.libs/libsimplegl.a

__top_builddir__bin_fire_SOURCES = fire.c
__top_builddir__bin_fire_CFLAGS  = -D_THREAD_SAFE -DHAVE_OPENGL
__top_builddir__bin_fire_LDFLAGS = -lm -l3dmath -lSDL2 -framework OpenGL $(top_builddir)/lib/.libs/libsimplegl.a
endif
