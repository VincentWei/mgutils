TOP_DIR=..
AM_CPPFLAGS=-I$(TOP_DIR)/include

SUBDIRS = @MGUTILS_SUBDIRS@
DIST_SUBDIRS = mywins skin vcongui templates 

lib_LTLIBRARIES = libmgutils@LIB_SUFFIX@.la

libmgutils@LIB_SUFFIX@_la_LIBADD = @MGUTILS_MODULES@ @DEP_LIBS@
libmgutils@LIB_SUFFIX@_la_DEPENDENCIES = @MGUTILS_MODULES@
libmgutils@LIB_SUFFIX@_la_SOURCES = 
libmgutils@LIB_SUFFIX@_la_LDFLAGS = -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) 

EXTRA_DIST=

CLEANFILES=*.a
