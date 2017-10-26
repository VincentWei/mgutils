if MGUTILS_NOSUFFIX
SAMPLES=samples
else
SAMPLES=
endif

SUBDIRS = src include $(SAMPLES) m4 cmake

# The directory where the include files will be installed
libmgutilsincludedir =

# Which header files to install
libmgutilsinclude_HEADERS =

EXTRA_DIST = INSTALL README

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mgutils.pc

