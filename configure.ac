AC_INIT([libsimplegl], [0.1], [manvscode@manvscode.com], [libsimplegl], [http://www.manvscode.com/])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AM_SILENT_RULES([yes])
AC_CONFIG_HEADERS([src/libsimplegl-config.h])

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_C_INLINE
AC_HEADER_STDC
#AC_LANG([C++])
#AX_CXX_COMPILE_STDCXX_11 # C++11 support
AC_PROG_RANLIB


AH_TOP([
#ifndef _LIBSIMPLEGL_H_
#define _LIBSIMPLEGL_H_
])
AH_BOTTOM([
#endif /* _LIBSIMPLEGL_H_ */
])

# If big endian define WORDS_BIGENDIAN
AC_C_BIGENDIAN

DFLAGS="-D SIMPLEGL_DEBUG"

CFLAGS="-Wall -g -ggdb -O0 $DFLAGS -I/usr/include -I/usr/local/include -I/usr/include"
#CXXFLAGS="-Wall -g -ggdb -O0 $DFLAGS -I/usr/include -I/usr/local/include -I/usr/include"
#LDFLAGS="-lGL -lGLU -lm"
#LDFLAGS="$LDFLAGS -L/System/Library/Frameworks/OpenGL.framework/Libraries -framework GLUT -framework OpenGL"


AC_CHECK_HEADERS([float.h])
AC_CHECK_HEADERS([math.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([sqrtf])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([sqrtl])
AC_FUNC_MALLOC
AC_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_PREREQ
AC_PROG_AWK
AC_PROG_LN_S
AC_PROG_MAKE_SET


AM_PROG_AR
LT_INIT([shared static])

AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

AC_PROG_INSTALL

echo " ------------------------------ "
echo ""
echo "   CFLAGS = $CFLAGS"
echo " CXXFLAGS = $CFLAGS"
echo "  LDFLAGS = $LDFLAGS"
echo ""
echo " ------------------------------ "

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        tests/Makefile
])

AC_OUTPUT

