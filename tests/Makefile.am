AM_CFLAGS = -std=c99 -pg -g -ggdb -O0 -I$(top_builddir)/src/ -I. -I.. -I/usr/local/include/ -L$(top_builddir)/lib/ 
AM_CXXFLAGS = -std=c++0x -pg -g -ggdb -O0 -I$(top_builddir)/src/ -I. -I.. -I/usr/local/include/ -L$(top_builddir)/lib/
LDADD = -limageio #-lGL

bin_PROGRAMS = \
$(top_builddir)/bin/test-shaders \
$(top_builddir)/bin/test-axes

__top_builddir__bin_test_shaders_SOURCES = test-shaders.c
__top_builddir__bin_test_shaders_CFLAGS  = -D_THREAD_SAFE -DHAVE_OPENGL
__top_builddir__bin_test_shaders_LDFLAGS = -lm -l3dmath -lSDL2 -framework OpenGL $(top_builddir)/lib/.libs/libsimplegl.a

__top_builddir__bin_test_axes_SOURCES = test-axes.c
__top_builddir__bin_test_axes_CFLAGS  = -D_THREAD_SAFE -DHAVE_OPENGL
__top_builddir__bin_test_axes_LDFLAGS = -lm -l3dmath -lSDL2 -framework OpenGL $(top_builddir)/lib/.libs/libsimplegl.a
