if (WIN32)
    mg_add_sub_source_dirs (
        mywins
        templates
        )
        
    include_directories (
        "${CMAKE_CURRENT_SOURCE_DIR}/mywins"
        "${CMAKE_CURRENT_SOURCE_DIR}/templates"
        )
else ()
    mg_add_sub_source_dirs (
        mywins
        skin
        vcongui
        templates
        )   

    include_directories (
        "${CMAKE_CURRENT_SOURCE_DIR}/mywins"
        "${CMAKE_CURRENT_SOURCE_DIR}/skin"
        "${CMAKE_CURRENT_SOURCE_DIR}/vcongui"
        "${CMAKE_CURRENT_SOURCE_DIR}/templates"
        )
endif (WIN32) 

################ make & install for dynamic library #####################
if (NOT disable_shared)
    # shared
    add_library (${MGUTILS_LIBNAME} SHARED ${_source_list_})
    set_target_properties (${MGUTILS_LIBNAME} PROPERTIES VERSION ${MGUTILS_VERSION} SOVERSION ${MGUTILS_SOVERSION})
else ()
    # static
    add_library (${MGUTILS_LIBNAME} STATIC ${_source_list_})
endif (NOT disable_shared)
set_target_properties (${MGUTILS_LIBNAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
target_link_libraries (${MGUTILS_LIBNAME} ${MINIGUI_LIBRARIES})
install (TARGETS ${MGUTILS_LIBNAME}
    LIBRARY DESTINATION lib     # for .so  (dynamic library)
    ARCHIVE DESTINATION lib     # for .lib (import library)
    RUNTIME DESTINATION bin     # for .dll (dynamic library)
    )
