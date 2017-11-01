cmake_minimum_required (VERSION 2.6)

project (mGUtils)

# define if this is a CMAKE PROJECT
add_definitions (-D__CMAKE_PROJECT__)
# define if this is a MGUTILS project
add_definitions (-D__MGUTILS_LIB__)

set (MGUTILS_MAJOR_VERSION 1)
set (MGUTILS_MINOR_VERSION 0)
set (MGUTILS_MICRO_VERSION 5)

set (MGUTILS_NAME "mgutils")
set (MGUTILS_VERSION ${MGUTILS_MAJOR_VERSION}.${MGUTILS_MINOR_VERSION}.${MGUTILS_MICRO_VERSION})
set (MGUTILS_SOVERSION ${MGUTILS_MAJOR_VERSION})

set (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/minigui/cmake/Modules" "${CMAKE_MODULE_PATH}")
include (common)

########################### check header and function ########################
include (CheckIncludeFiles)
check_include_files (fcntl.h HAVE_FCNTL_H)
check_include_files (stdlib.h HAVE_STDLIB_H)
check_include_files (string.h HAVE_STRING_H)
check_include_files (sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_files (sys/time.h HAVE_SYS_TIME_H)
check_include_files (termio.h HAVE_TERMIO_H)
check_include_files (termios.h HAVE_TERMIOS_H)
check_include_files (unistd.h HAVE_UNISTD_H)

include (CheckFunctionExists)
check_function_exists (bzero HAVE_BZERO)
check_function_exists (dup2 HAVE_DUP2)
check_function_exists (getcwd HAVE_GETCWD)
check_function_exists (memmove HAVE_MEMMOVE)
check_function_exists (memset HAVE_MEMSET)
check_function_exists (putenv HAVE_PUTENV)
check_function_exists (strchr HAVE_STRCHR)
check_function_exists (strdup HAVE_STRDUP)
check_function_exists (strerror HAVE_STRERROR)
check_function_exists (strstr HAVE_STRSTR)

########################## options #########################
mg_declare_option (with_libsuffix      STRING  ""  "configure the suffix of the library name.")
#mg_set_value (LIB_SUFFIX with_libsuffix IF with_libsuffix)
if (NOT with_libsuffix STREQUAL "")
    set (LIB_SUFFIX "${with_libsuffix}")
    set (MGUTILS_LIBNAME ${MGUTILS_NAME}_${LIB_SUFFIX})
else ()
    set (MGUTILS_LIBNAME ${MGUTILS_NAME})
endif (NOT with_libsuffix STREQUAL "")

option(disable_shared "Disable shared library" OFF)

######################### conditional #####################
if (MINIGUI_LIBRARIES MATCHES "minigui_procs")
    set (_MGRM_PROCESSES ON)
    set (MGRM_PROCESSES ON)
endif (MINIGUI_LIBRARIES MATCHES "minigui_procs")

##################### find minigui #######################
INCLUDE (FindPkgConfig)
PKG_CHECK_MODULES (MINIGUI minigui)

if (NOT MINIGUI_FOUND)
    message(FATAL_ERROR 
       "MiniGUI is not properly installed on the system. You need 
        MiniGUI Ver 3.0.x or later for building this package. 
        Please configure and install MiniGUI Ver 3.0.x first."
        )
endif (NOT MINIGUI_FOUND)

include_directories (${MINIGUI_INCLUDE_DIRS})
link_directories (${MINIGUI_LIBRARY_DIRS})
include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/include")

###################### add sub directories #########################
add_subdirectory (src)
# add_subdirectory (samples)

# for makefile.ng to get source files
#FILE(WRITE tmp_files "files=\" ${_source_list_}\"")
FILE(WRITE files "files= ${_source_list_}
project_name=${MGUTILS_NAME}
lib_name=lib${MGUTILS_NAME}
lib_major_version=${MGUTILS_MAJOR_VERSION}
lib_minor_version=${MGUTILS_MINOR_VERSION}
lib_micro_version=${MGUTILS_MICRO_VERSION}\n")


##################### files install #########################
install (DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION include/${MGUTILS_NAME}
    FILES_MATCHING PATTERN "*.h"
                   PATTERN ".svn" EXCLUDE
    )

# Cmake Modules
install (DIRECTORY "${PROJECT_SOURCE_DIR}/cmake/"
    DESTINATION share/minigui/cmake/Modules
    FILES_MATCHING PATTERN "*.cmake"
    PATTERN "*private*" EXCLUDE
    PATTERN ".svn" EXCLUDE
    )

# .pc
LIST (APPEND PC_LIBS_LIST ${MGUTILS_LIBNAME})
SET (prefix ${CMAKE_INSTALL_PREFIX})
SET (exec_prefix "\${prefix}")
SET (libdir "\${exec_prefix}/lib")
SET (includedir "\${prefix}/include")
FOREACH (i ${PC_LIBS_LIST})
    SET (PC_LIBS "${PC_LIBS} -l${i}")
ENDFOREACH (i ${PC_LIBS_LIST})
SET (PC_FILE "${PROJECT_BINARY_DIR}/${MGUTILS_NAME}.pc")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/mgutils.pc.in" ${PC_FILE})
INSTALL (FILES ${PC_FILE} DESTINATION lib/pkgconfig)
