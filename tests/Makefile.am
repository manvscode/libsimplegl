#AM_CFLAGS = -std=c99 -Wall -pedantic -O3 -I$(top_builddir)/src/ -I/usr/local/include/ -L$(top_builddir)/lib/  -L/usr/local/lib
AM_CFLAGS = -std=c99 -pg -g -ggdb -O0 -I$(top_builddir)/src/ -I. -I.. -I/usr/local/include/ -L$(top_builddir)/lib/  -L/usr/local/lib -L/usr/lib64
AM_CXXFLAGS = -std=c++0x -pg -g -ggdb -O0 -I$(top_builddir)/src/ -I. -I.. -I/usr/local/include/ -L$(top_builddir)/lib/  -L/usr/local/lib -L/usr/lib64
LDADD = -limageio -lfreeglut -lGL

bin_PROGRAMS = \
$(top_builddir)/bin/test-shaders

__top_builddir__bin_test_shaders_SOURCES = test-shaders.c
__top_builddir__bin_test_shaders_LDADD   = -lGL -lglut $(top_builddir)/lib/.libs/libsimplegl.a

