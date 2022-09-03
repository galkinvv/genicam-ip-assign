cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to RelWithDebInfo as none was specified.")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "RelWithDebInfo" "Debug" "Release" "MinSizeRel")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}") # for FindGenicamArena.cmake

set(CMAKE_INSTALL_RPATH "$ORIGIN") #search .so at runtime via relative path near executable

project(genicam_ip_assign)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(GenicamArena REQUIRED)


add_library(genicam_ip_assign_lib STATIC "genicam-ip-assign.cpp")
target_link_libraries(genicam_ip_assign_lib PUBLIC GenicamArena)

add_executable(genicam_ip_assign "main.cpp")
target_link_libraries(genicam_ip_assign PUBLIC genicam_ip_assign_lib)

install(TARGETS genicam_ip_assign 
        RUNTIME DESTINATION bin
)

# install runtime dependencies to get folder with portable binary
foreach(ARENA_LIB ${GenicamArena_DEP_LIBRARIES} ${GenicamArena_LIBRARY})
    install(FILES "${ARENA_LIB}" DESTINATION bin)
    if(IS_SYMLINK "${ARENA_LIB}")
        get_filename_component(ARENA_LIB_exact "${ARENA_LIB}" REALPATH) # resolve symlink
        install(FILES "${ARENA_LIB_exact}" DESTINATION bin)
    endif()
endforeach()

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    # use RUNPATH instead of RPATH to allow LD_PRELOAD, force link to all libraies to make them
    # findable via RUNPATH as dependencies of 'intermediate' imported libs (like libarena.so.0=>libGCBase_gcc54_v3_3_LUCID.so)
    target_link_libraries(genicam_ip_assign PRIVATE "-Wl,--enable-new-dtags" "-Wl,-rpath=$ORIGIN" "-Wl,--no-as-needed")
    target_compile_options(genicam_ip_assign PRIVATE "-Wall")
    target_compile_options(genicam_ip_assign_lib PRIVATE "-Wall")
endif()

if(EXISTS "/usr/src/googletest/googletest/src/gtest-all.cc")
    find_package(Threads REQUIRED)
    add_executable(genicam_ip_assign_smoke_test "smoke_test.cpp" "/usr/src/googletest/googletest/src/gtest-all.cc" "/usr/src/googletest/googletest/src/gtest_main.cc")
    target_include_directories(genicam_ip_assign_smoke_test PRIVATE "/usr/src/googletest/googletest/")
    target_link_libraries(genicam_ip_assign_smoke_test PUBLIC genicam_ip_assign_lib "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(genicam_ip_assign_smoke_test PRIVATE "-Wl,--enable-new-dtags" "-Wl,-rpath=$ORIGIN" "-Wl,--no-as-needed")
    target_compile_options(genicam_ip_assign_smoke_test PRIVATE "-Wall")

    install(TARGETS genicam_ip_assign_smoke_test 
            RUNTIME DESTINATION bin
    )
endif()
