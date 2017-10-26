#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

MGUTILS_NAME="mgutils"
MGUTILS_MAJOR_VERSION=1
MGUTILS_MINOR_VERSION=0
MGUTILS_MICRO_VERSION=5
MGUTILS_INTERFACE_AGE=0
MGUTILS_BINARY_AGE=0

MGUTILS_VERSION=$MGUTILS_MAJOR_VERSION.$MGUTILS_MINOR_VERSION.$MGUTILS_MICRO_VERSION

AC_PREREQ(2.60)
AC_INIT(lib$MGUTILS_NAME, $MGUTILS_VERSION)
AC_CONFIG_SRCDIR(src/mywins/colordlg.c)

AC_SUBST(MGUTILS_MAJOR_VERSION)
AC_SUBST(MGUTILS_MINOR_VERSION)
AC_SUBST(MGUTILS_MICRO_VERSION)
AC_SUBST(MGUTILS_INTERFACE_AGE)
AC_SUBST(MGUTILS_BINARY_AGE)
AC_SUBST(MGUTILS_VERSION)

# libtool versioning
LT_RELEASE=$MGUTILS_MAJOR_VERSION.$MGUTILS_MINOR_VERSION
LT_CURRENT=`expr $MGUTILS_MICRO_VERSION - $MGUTILS_INTERFACE_AGE`
LT_REVISION=$MGUTILS_INTERFACE_AGE
LT_AGE=`expr $MGUTILS_BINARY_AGE - $MGUTILS_INTERFACE_AGE`
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE

dnl ========================================================================
# Checks for programs.
AC_PROG_CC
AC_PROG_LD
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl ========================================================================
dnl Put the name and version of the package here

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h termio.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero dup2 getcwd memmove memset putenv strchr strdup strerror strstr])

AC_ARG_WITH(libsuffix,
  [  --with-libsuffix=ARG    configure the suffix of the library name.])

if test "x$with_libsuffix" = "x"; then
  LIB_SUFFIX=""
else
  LIB_SUFFIX="_$with_libsuffix"
fi

MGUTILS_LIBNAME="${MGUTILS_NAME}${LIB_SUFFIX}"

have_libminigui="no"
AC_CHECK_HEADERS(minigui/common.h, have_libminigui=yes, foo=bar)

dnl ========================================================================
dnl check for run-time mode of MiniGUI
dnl ========================================================================
procs_version="no"
AC_CHECK_DECLS(_MGRM_PROCESSES, procs_version="yes", foo=bar, [#include <minigui/common.h>])

CFLAGS="$CFLAGS -D__MGUTILS_LIB__"

AM_CONDITIONAL(MGRM_PROCESSES, test "x$procs_version" = "xyes")
AM_CONDITIONAL(MGUTILS_NOSUFFIX, test "x$with_libsuffix" = "x")

if test "$ac_cv_prog_gcc" = "yes"; then
    CFLAGS="$CFLAGS -Wall -Wstrict-prototypes -pipe"
fi

AC_SUBST(LIB_SUFFIX)

PKG_CHECK_MODULES([MINIGUI], [minigui >= 3.0.10])
APP_LIBS="$APP_LIBS $MINIGUI_LIBS"
AC_SUBST(APP_LIBS)

PC_LIBS="-l${MGUTILS_LIBNAME}"
PC_CFLAGS=""
AC_SUBST(PC_LIBS)
AC_SUBST(PC_CFLAGS)

AC_OUTPUT([mgutils.pc
         Makefile
         include/Makefile
         cmake/Makefile
         src/Makefile
         src/mywins/Makefile
         src/vcongui/Makefile
         src/skin/Makefile
         src/templates/Makefile
         samples/Makefile
         samples/mginit/Makefile
         samples/mginit/res/Makefile
         samples/templates/Makefile
         samples/mywins/Makefile
         samples/skin/Makefile
         samples/notebook/Makefile
         samples/notebook/res/Makefile
         samples/painter/Makefile
         samples/picview/Makefile
         samples/picview/res/Makefile
         samples/vcongui/Makefile
         m4/Makefile])

if test "x$have_libminigui" != "xyes"; then
    AC_MSG_WARN([
        MiniGUI is not properly installed on the system. You need 
        MiniGUI Ver 3.0.x or later for building this package. 
        Please configure and install MiniGUI Ver 3.0.x first.
    ])
fi

