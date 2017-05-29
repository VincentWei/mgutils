TOP_DIR=..
INCLUDES = -I$(TOP_DIR)/include

SUBDIRS = mywins skin vcongui templates 

lib_LTLIBRARIES = libmgutils@LIB_SUFFIX@.la

COMMON_LADD =               \
    mywins/libmywins.la     \
    vcongui/libvcongui.la   \
    skin/libskin.la     \
    templates/libtemplates.la 

libmgutils@LIB_SUFFIX@_la_LIBADD = $(COMMON_LADD)
libmgutils@LIB_SUFFIX@_la_SOURCES = 
libmgutils@LIB_SUFFIX@_la_LDFLAGS = -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

EXTRA_DIST=

CLEANFILES=*.a
