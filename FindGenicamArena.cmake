include(FindPackageHandleStandardArgs)
if (UNIX AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    get_filename_component(GenicamArena_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/../ArenaSDK_Linux_x64/lib64/libarena.so.0" ABSOLUTE)
    if (EXISTS "${GenicamArena_LIBRARY}")
        if(NOT TARGET GenicamArena)
            add_library(GenicamArena SHARED IMPORTED)
            file(GLOB GenicamArena_DEP_LIBRARIES CONFIGURE_DEPENDS
                "${CMAKE_CURRENT_LIST_DIR}/../ArenaSDK_Linux_x64/GenICam/library/lib/Linux64_x64/lib*_gcc54_v3_3_LUCID.so")
            list(APPEND GenicamArena_DEP_LIBRARIES
                "${CMAKE_CURRENT_LIST_DIR}/../ArenaSDK_Linux_x64/lib64/liblucidlog.so.0"
                "${CMAKE_CURRENT_LIST_DIR}/../ArenaSDK_Linux_x64/lib64/libgentl.so.0")
            set_target_properties(GenicamArena PROPERTIES
                IMPORTED_LOCATION "${GenicamArena_LIBRARY}"
                INTERFACE_LINK_LIBRARIES "${GenicamArena_DEP_LIBRARIES}"
                INTERFACE_INCLUDE_DIRECTORIES
                    "${CMAKE_CURRENT_LIST_DIR}/../ArenaSDK_Linux_x64/include;${CMAKE_CURRENT_LIST_DIR}/../ArenaSDK_Linux_x64/GenICam/library/CPP/include"
                IMPORTED_GLOBAL True
                )
        endif()
    else()
        message(WARNING "GenicamArena library not found in ${GenicamArena_LIBRARY}")
    endif()
else()
    message(WARNING "GenicamArena finding not implemented for this platform in ${CMAKE_CURRENT_LIST_FILE}")
endif()
find_package_handle_standard_args(GenicamArena REQUIRED_VARS GenicamArena_LIBRARY GenicamArena_DEP_LIBRARIES)
