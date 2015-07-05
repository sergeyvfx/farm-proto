###########################################################################
# Disable insource build

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR
		"CMake generation for Cycles is not allowed within the source directory!"
		"\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
		"\n "
		"\n rm CMakeCache.txt"
		"\n cd .."
		"\n mkdir cmake-make"
		"\n cd cmake-make"
		"\n cmake ../cycles"
	)
endif()

###########################################################################
# Policies

cmake_minimum_required(VERSION 2.8)

# So library linking is more sane.
cmake_policy(SET CMP0003 NEW)

# So syntax problems are errors.
cmake_policy(SET CMP0010 NEW)

# So BUILDINFO and BLENDERPATH strings are automatically quoted.
cmake_policy(SET CMP0005 NEW)

# Input directories must have CMakeLists.txt
cmake_policy(SET CMP0014 NEW)

if(NOT (${CMAKE_VERSION} VERSION_LESS 3.0))
	# Keep until CMake-3.0 is min requirement.
	cmake_policy(SET CMP0043 OLD)
endif()

###########################################################################
# Macros and utilities.

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/src/cmake"
	"${CMAKE_SOURCE_DIR}/src/cmake/Modules"
)
include("${CMAKE_SOURCE_DIR}/src/cmake/macros.cmake")

###########################################################################
# Global settings.

# Avoid having empty buildtype.
set(CMAKE_BUILD_TYPE_INIT "Release")


# Global compile definitions since add_definitions() adds for all.
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG           DEBUG _DEBUG)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE         NDEBUG)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL      NDEBUG)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO  NDEBUG)

###########################################################################
# Initialize project.

project(farm-prototype)

###########################################################################
# Options.

option(WITH_CPP11 "Build with C++11 standard enabled" ON)

###########################################################################
# Configuration.

include(CheckCXXCompilerFlag)
include(configure_build)

###########################################################################
# Include directories.

include_directories(
)

###########################################################################
# Sources.

add_subdirectory(src)
add_subdirectory(third_party)
