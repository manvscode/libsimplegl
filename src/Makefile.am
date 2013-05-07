# Add new files in alphabetical order. Thanks.
libsimplegl_src = vec3.c vec4.c

# Add new files in alphabetical order. Thanks.
libsimplegl_headers = vec3.h vec4.h

library_includedir      = $(includedir)/libsimplegl/
library_include_HEADERS = $(libsimplegl_headers)

# Library
lib_LTLIBRARIES                            = $(top_builddir)/lib/libsimplegl.la
__top_builddir__lib_libsimplegl_la_SOURCES = $(libsimplegl_src)
__top_builddir__lib_libsimplegl_la_LIBADD  = -lGL -lm

