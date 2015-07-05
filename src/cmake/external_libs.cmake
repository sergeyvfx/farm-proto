###########################################################################
# Precompiled libraries tips and hints, for find_package().

INCLUDE(FindPkgConfig)

###########################################################################
# LibSOUP

pkg_check_modules(GLIB2 glib-2.0)
pkg_check_modules(LIBSOUP libsoup-2.4)

###########################################################################
# GFlags/GLog

find_package(Glog REQUIRED)
find_package(Gflags REQUIRED)

###########################################################################
# PThreads

find_package(Threads)
set(PTHREADS_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
