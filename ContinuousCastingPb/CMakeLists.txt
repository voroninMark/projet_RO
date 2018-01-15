CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(CC-SIMULATOR CXX C)

#
 # CPack configuration
#

INCLUDE(CMakeCPack.cmake)

#
 # Debug mode
#

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

  if ("${CMAKE_BUILD_TYPE}" EQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" EQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
  endif ()
endif ()

#
 # Modules
#

INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CMakeDetermineCCompiler)

#
 # Check libraries with pkgconfig
#

#FIND_PACKAGE(PkgConfig REQUIRED)
#PKG_CHECK_MODULES(ARTIS REQUIRED artis-star-1.0)
SET(ARTIS_INCLUDE_DIRS "/etudiants/mvoronin/Documents/projet_RO/ContinuousCastingPb/usr/local/include/artis-star-1.0")
SET(ARTIS_LIBRARIES "/etudiants/mvoronin/Documents/projet_RO/ContinuousCastingPb/usr/local/bin/artis-star-1.0")
#
 # Generate the config.h
#

CHECK_INCLUDE_FILE_CXX(cxxabi.h ARTIS_HAVE_GCC_ABI_DEMANGLE)
CHECK_INCLUDE_FILE_CXX(execinfo.h ARTIS_HAVE_EXECINFO)
CHECK_INCLUDE_FILE_CXX(signal.h ARTIS_HAVE_SIGNAL)

#
 # Browse the src subdirectory
#

ADD_SUBDIRECTORY(src)

#
 # Status
#

MESSAGE(STATUS "- - - -")
MESSAGE(STATUS "Project configured successfully")
MESSAGE(STATUS "Using ${CMAKE_INSTALL_PREFIX} for installation")
MESSAGE(STATUS "Build type ${CMAKE_BUILD_TYPE}")
IF (ARTIS_HAVE_GCC_ABI_DEMANGLE)
  MESSAGE(STATUS "Build with GCC ABI Demangle...: yes")
ENDIF (ARTIS_HAVE_GCC_ABI_DEMANGLE)
IF (ARTIS_HAVE_EXECINFO)
  MESSAGE(STATUS "Build with execinfo.h.........: yes")
ENDIF (ARTIS_HAVE_EXECINFO)
IF (Boost_FOUND)
  MESSAGE(STATUS "Build with boost..............: yes")
ENDIF (Boost_FOUND)
MESSAGE(STATUS "- - - -")

# vim:tw=0:ts=8:tw=0:sw=2:sts=2
