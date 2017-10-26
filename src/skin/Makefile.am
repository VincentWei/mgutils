TOP_DIR=../..
AM_CPPFLAGS=-I./ -I../../include 

noinst_LTLIBRARIES = libskin.la

SRC_FILES =  skin.c  \
	cmdbutton.c  chkbutton.c  label.c    bmplabel.c  slider.c   rotslider.c   mgcontrol.c

HDR_FILES = item_comm.h  \
	cmdbutton.h  chkbutton.h  label.h    bmplabel.h  slider.h   rotslider.h   mgcontrol.h

libskin_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libskin
