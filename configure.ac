AC_PREREQ(2.67)
AC_INIT([simplegl], [0.2.0], [manvscode@manvscode.com], [simplegl], [http://www.manvscode.com/])
AM_INIT_AUTOMAKE([1.13 foreign dist-zip silent-rules -Wall -Werror])

AM_SILENT_RULES([yes])
AC_CONFIG_HEADERS([src/libsimplegl-config.h])

AC_PROG_CC
AC_PROG_CC_STDC
AC_C_INLINE
AC_HEADER_STDC


AH_TOP([
#ifndef _LIBSIMPLEGL_H_
#define _LIBSIMPLEGL_H_
])
AH_BOTTOM([
#endif /* _LIBSIMPLEGL_H_ */
])

#DFLAGS="-D GL_GLEXT_PROTOTYPES -D SIMPLEGL_DEBUG"
DFLAGS="-D GL_GLEXT_PROTOTYPES $DFLAGS"

CFLAGS="$CFLAGS $DFLAGS "
LDFLAGS="$LDFLAGS"

AC_CHECK_HEADERS([float.h])
AC_CHECK_HEADERS([math.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([sqrtf])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([sqrtl])
AC_HEADER_STDBOOL


AC_MSG_NOTICE([Checking libraries...])

AC_CHECK_LIB([m], [cos],
	[],
	[AC_MSG_ERROR([Missing math library (i.e. -lm).])]
)
AC_CHECK_LIB([m3d], [m3d_clampf],
	[have_m3d=yes],
	[AC_MSG_ERROR([Missing libm3d library (i.e. -lm3d).])]
)
AC_CHECK_LIB([GL], [glGetString],
	[have_opengl=yes],
	[AC_MSG_ERROR([Missing OpenGL library (i.e. -lGL).])]
)
AC_CHECK_LIB([X11], [XOpenDisplay],
	[have_x11=yes],
	[AC_MSG_ERROR([Missing X11 library (i.e. -lX11).])]
)

AC_MSG_NOTICE([---------------------------------------------------------------------])
PKG_CHECK_MODULES([GL], [gl])
AC_MSG_NOTICE([---------------------------------------------------------------------])


AM_PROG_AR
LT_INIT([shared static])

AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

AC_PROG_INSTALL

AC_ARG_ENABLE([tests],
	[AS_HELP_STRING([--disable-tests], [Enable test programs.])],
	[:],
	[enable_tests=no])

AM_CONDITIONAL([HAVE_LIBM3D], [test "$have_m3d" = "yes"])
AM_CONDITIONAL([HAVE_LIBGL], [test "$have_opengl" = "yes"])
AM_CONDITIONAL([HAVE_LIBX11], [test "$have_x11" = "yes"])
AM_CONDITIONAL([ENABLE_TESTS], [test "$enable_tests" = "yes"])

PKG_CHECK_MODULES([M3D], [m3d],
	[AC_MSG_NOTICE([Found libm3d])],
	[AC_MSG_ERROR([Missing libm3d])],
)

PKG_CHECK_MODULES([IMAGEIO], [imageio],
	[AC_MSG_NOTICE([Found libimageio])],
	[AC_MSG_ERROR([Missing libimageio])],
)


if test "x$enable_tests" = "xyes"; then
	PKG_CHECK_MODULES([SDL2], [sdl2],
		[AC_MSG_NOTICE([Found libsdl2])],
		[AC_MSG_ERROR([Missing libsdl2])],
	)
fi

echo "------------------------------------------------------------------------------"
if [test -z "$CFLAGS"]; then
	echo "  CFLAGS: Not set"
else
	echo "  CFLAGS: $CFLAGS"
fi
if [test -z "$LDFLAGS"]; then
	echo " LDFLAGS: Not set"
else
	echo " LDFLAGS: $LDFLAGS"
fi
echo "------------------------------------------------------------------------------"


AC_CONFIG_FILES([
	Makefile
	src/Makefile
	tests/Makefile
	simplegl.pc
])

AC_OUTPUT

