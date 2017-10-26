# - This module determines the mgutils library of the system
# The following variables are set if the library found:
# MGUTILS_FOUND - If false do nnt try to use mgutils.
# MGUTILS_INCLUDE_DIR - where to find the headfile of library.
# MGUTILS_LIBRARY_DIR - where to find the mgutils library.
# MGUTILS_LIBRARY - the library needed to use mgutils.

# find the headfile of library

set (MGUTILS_HEADS mgutils/mgutils.h)
find_path (MGUTILS_INCLUDE_DIR ${MGUTILS_HEADS})

set (MGUTILS_NAMES mgutils libmgutils)
find_library (MGUTILS_LIBRARY NAMES ${MGUTILS_NAMES})

# just find one of dependency, guess other one.
if (NOT MGUTILS_LIBRARY AND MGUTILS_INCLUDE_DIR)
	message ("We just find the headfile, try to guess the library location.")
	set (MGUTILS_LIBRARY_DIR "${MGUTILS_INCLUDE_DIR}/../lib")
	find_library (MGUTILS_LIBRARY NAMES ${MGUTILS_NAMES} PATHS ${MGUTILS_LIBRARY_DIR})
elseif (NOT MGUTILS_INCLUDE_DIR AND MGUTILS_LIBRARY)
	message ("We just find the lib file, try to guess the include location.")
	string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" MGUTILS_LIBRARY_DIR ${MGUTILS_LIBRARY})
	find_path (MGUTILS_INCLUDE_DIR ${MGUTILS_HEADS} "${MGUTILS_LIBRARY_DIR}../included")
endif()

# find the library.
if (MGUTILS_INCLUDE_DIR AND MGUTILS_LIBRARY)
	set (MGUTILS_FOUND TRUE)
	if (NOT MGUTILS_LIBRARY_DIR)
		string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" MGUTILS_LIBRARY_DIR ${MGUTILS_LIBRARY})
	endif ()
    string (REGEX MATCH "[a-z|A-Z|0-9|_|-]+[.](a|so|lib|dll|LIB|DLL)[^/]*$" MGUTILS_LIBRARY_WHOLE_NAME "${MGUTILS_LIBRARY}")
    string (REGEX MATCH "[^(lib)][a-z|A-Z|0-9|_|-]+" MGUTILS_LIBRARIES ${MGUTILS_LIBRARY_WHOLE_NAME})
	message (STATUS "Find MGUTILS include in ${MGUTILS_INCLUDE_DIR}")
	message (STATUS "Find MGUTILS library in ${MGUTILS_LIBRARY}")
else ()
	message (STATUS "Could NOT find MGUTILS, (missing: MGUTILS_INCLUDE_DIR MGUTILS_LIBRARY)")
endif ()

