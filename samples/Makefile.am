COMMON_SUBDIRS = templates notebook painter picview

if MGRM_PROCESSES
PROCS_SUBDIRS=mginit
else
PROCS_SUBDIRS=
endif

DIST_SUBDIRS = $(COMMON_SUBDIRS) vcongui skin mywins mginit
SUBDIRS = @MGUTILS_SUBDIRS@ $(COMMON_SUBDIRS) $(PROCS_SUBDIRS) 
