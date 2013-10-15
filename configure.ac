AC_PREREQ(2.67)
AC_INIT([libsimplegl], [0.2], [manvscode@manvscode.com], [libsimplegl], [http://www.manvscode.com/])
AM_INIT_AUTOMAKE([1.13 foreign silent-rules -Wall -Werror])

AM_SILENT_RULES([yes])
AC_CONFIG_HEADERS([src/libsimplegl-config.h])

AC_PROG_CC
AC_PROG_CC_STDC
AC_C_INLINE
AC_HEADER_STDC
AC_PROG_RANLIB


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
#CFLAGS="-Wall -g -ggdb -O0 $DFLAGS -I/usr/include -I/usr/local/include "
LDFLAGS="$LDFLAGS -l3dmath -lm -framework OpenGL -framework OpenGLES"


AC_CHECK_HEADERS([float.h])
AC_CHECK_HEADERS([math.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([sqrtf])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([sqrtl])
AC_HEADER_STDBOOL


AM_PROG_AR
LT_INIT([shared static])

AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

AC_PROG_INSTALL

echo " ------------------------------ "
echo ""
echo "   CFLAGS = $CFLAGS"
echo "  LDFLAGS = $LDFLAGS"
echo ""
echo " ------------------------------ "

AC_ARG_ENABLE([tests],
	[AS_HELP_STRING([--disable-tests], [Enable test programs.])],
	[:],
	[enable_tests=no])

AM_CONDITIONAL([ENABLE_TESTS], [test "$enable_tests" = "yes"])


AC_CONFIG_FILES([
        Makefile
        src/Makefile
        tests/Makefile
])

AC_OUTPUT

