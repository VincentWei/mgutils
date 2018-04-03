#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)
AC_INIT(libmgutils, 1.2.0)
AC_CONFIG_SRCDIR(src/templates/colordlg.c)

MGUTILS_NAME="mgutils"

dnl Set various version strings
#
# Making releases:
# Change the version, then:
#   _MICRO_VERSION += 1;
#   _INTERFACE_AGE += 1;
#   _BINARY_AGE += 1;
# if any functions have been added, set _INTERFACE_AGE to 0.
# if backwards compatibility has been broken,
# set _BINARY_AGE and _INTERFACE_AGE to 0.
#
MGUTILS_MAJOR_VERSION=1
MGUTILS_MINOR_VERSION=2
MGUTILS_MICRO_VERSION=0
MGUTILS_INTERFACE_AGE=0
MGUTILS_BINARY_AGE=0

MGUTILS_VERSION=$MGUTILS_MAJOR_VERSION.$MGUTILS_MINOR_VERSION.$MGUTILS_MICRO_VERSION

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

AC_CONFIG_HEADER(mgutilsconfig.h)

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

devel_mode="no"
detail_debug="no"

build_vcongui="no"
build_skin="yes"
build_mywins="yes"
build_dlgopenfile="yes"
build_dlgcolorsel="yes"
build_dlgfontsel="yes"
build_dlgshowinfo="yes"

MGUTILS_SUBDIRS="templates"
MGUTILS_MODULES="templates/libtemplates.la"

AC_ARG_ENABLE(develmode,
[  --enable-develmode       developer mode <default=no>],
devel_mode=$enableval)

AC_ARG_ENABLE(detaildebug,
[  --enable-detaildebug     detailed debug info <default=no>],
detail_debug=$enableval)

AC_ARG_ENABLE(vcongui,
[  --enable-vcongui         build VCOnGUI <default=yes>],
build_vcongui=$enableval)

AC_ARG_ENABLE(skin,
[  --enable-skin            build Skin <default=yes>],
build_skin=$enableval)

AC_ARG_ENABLE(mywins,
[  --enable-mywinds         build MyWins <default=yes>],
build_mywins=$enableval)

AC_ARG_ENABLE(dlgopenfile,
[  --enable-dlgopenfile     build Open File Dialog Box <default=yes>],
build_dlgopenfile=$enableval)

AC_ARG_ENABLE(dlgcolorsel,
[  --enable-dlgcolorsel     build Color Selection Dialog Box <default=yes>],
build_dlgcolorsel=$enableval)

AC_ARG_ENABLE(dlgfontsel,
[  --enable-dlgfontsel      build Font Selection Dialog Box <default=yes>],
build_dlgfontsel=$enableval)

AC_ARG_ENABLE(dlgshowinfo,
[  --enable-dlgshowinfo     build Show Info Dialog Box <default=yes>],
build_dlgshowinfo=$enableval)

if test "x$devel_mode" = "xyes"; then
    build_vcongui="yes"
fi

if test "x$build_vcongui" = "xyes"; then
  AC_DEFINE(_MGUTILS_VCONGUI, 1, [Define if include VCOnGUI])
  MGUTILS_SUBDIRS="$MGUTILS_SUBDIRS vcongui"
  MGUTILS_MODULES="$MGUTILS_MODULES vcongui/libvcongui.la"
fi

if test "x$build_skin" = "xyes"; then
  AC_DEFINE(_MGUTILS_SKIN, 1, [Define if include Skin])
  MGUTILS_SUBDIRS="$MGUTILS_SUBDIRS skin"
  MGUTILS_MODULES="$MGUTILS_MODULES skin/libskin.la"
fi

if test "x$build_mywins" = "xyes"; then
  AC_DEFINE(_MGUTILS_MYWINS, 1, [Define if include MyWins])
  MGUTILS_SUBDIRS="$MGUTILS_SUBDIRS mywins"
  MGUTILS_MODULES="$MGUTILS_MODULES mywins/libmywins.la"
fi

if test "x$build_dlgopenfile" = "xyes"; then
  AC_DEFINE(_MGUTILS_DLGOPENFILE, 1, [Define if include Open File Dialog Box])
fi

if test "x$build_dlgcolorsel" = "xyes"; then
  AC_DEFINE(_MGUTILS_DLGCOLORSEL, 1, [Define if include Color Selection Dialog Box])
fi

if test "x$build_dlgfontsel" = "xyes"; then
  AC_DEFINE(_MGUTILS_DLGFONTSEL, 1, [Define if include Font Selection Dialog Box])
fi

if test "x$build_dlgshowinfo" = "xyes"; then
  AC_DEFINE(_MGUTILS_DLGSHOWINFO, 1, [Define if include Show Info Dialog Box])
fi

AC_DEFINE_UNQUOTED(MGUTILS_MAJOR_VERSION, $MGUTILS_MAJOR_VERSION,
            [Major version of mGUtils])
AC_DEFINE_UNQUOTED(MGUTILS_MINOR_VERSION, $MGUTILS_MINOR_VERSION,
            [Minor version of mGUtils])
AC_DEFINE_UNQUOTED(MGUTILS_MICRO_VERSION, $MGUTILS_MICRO_VERSION,
            [Micro version of mGUtils])
AC_DEFINE_UNQUOTED(MGUTILS_INTERFACE_AGE, $MGUTILS_INTERFACE_AGE,
            [Interface age of mGUtils])
AC_DEFINE_UNQUOTED(MGUTILS_BINARY_AGE, $MGUTILS_BINARY_AGE,
            [Binary age of mGUtils])

AC_ARG_WITH(libsuffix,
[  --with-libsuffix=ARG     configure the suffix of the library name.])

if test "x$with_libsuffix" = "x"; then
  LIB_SUFFIX=""
else
  LIB_SUFFIX="_$with_libsuffix"
fi

MGUTILS_LIBNAME="${MGUTILS_NAME}${LIB_SUFFIX}"

dnl ========================================================================
dnl check for installation of MiniGUI
dnl ========================================================================
PKG_CHECK_MODULES([MINIGUI], [minigui >= 3.2.0])

AC_CHECK_DECLS(_MGRM_PROCESSES, minigui_runmode="procs", foo=bar, [#include <minigui/common.h>])
AM_CONDITIONAL(MGRM_PROCESSES, test "x$minigui_runmode" = "xprocs")
AM_CONDITIONAL(MGUTILS_NOSUFFIX, test "x$with_libsuffix" = "x")

CFLAGS="$CFLAGS -D__MGUTILS_LIB__"
if test "x$devel_mode" = "xyes"; then
    CPPFLAGS="$CPPFLAGS -D_DEBUG"
    if test "x$detail_debug" = "xyes"; then
        CPPFLAGS="$CPPFLAGS -DDEBUG"
    fi

    if test "$ac_cv_prog_gcc" = "yes"; then
        CPPFLAGS="$CPPFLAGS -Wall -Werror"
    fi
else
    CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

if test "$ac_cv_prog_gcc" = "yes"; then
    CFLAGS="$CFLAGS -O2 -Wstrict-prototypes -pipe"
fi

AC_SUBST(MGUTILS_SUBDIRS)
AC_SUBST(MGUTILS_MODULES)
AC_SUBST(LIB_SUFFIX)

DEP_LIBS="$DEP_LIBS $MINIGUI_LIBS"
APP_LIBS="$DEP_LIBS"

AC_SUBST(DEP_LIBS)
AC_SUBST(APP_LIBS)

PC_LIBS="-l${MGUTILS_LIBNAME} $DEP_LIBS"
PC_CFLAGS=""

AC_SUBST(PC_LIBS)
AC_SUBST(PC_CFLAGS)

AC_OUTPUT([
        mgutils.pc
        Makefile
        include/Makefile
        cmake/Makefile
        src/Makefile
        src/mywins/Makefile
        src/vcongui/Makefile
        src/skin/Makefile
        src/templates/Makefile
        m4/Makefile
])
