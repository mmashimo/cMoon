
cmake_minimum_required (VERSION 3.0)
project (cMoon VERSION 0.4)

# The version number.
# set (cMoon_VERSION_MAJOR 0)
# set (cMoon_VERSION_MINOR 4)

configure_file(cMoonConfig.h.in cMoonConfig.h)

# Source code directory (include files are also in it)
set (CMOON_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

# Make sure 'boost' is installed:
find_package(Boost)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
endif ()

if (APPLE)
  set (CMAKE_CXX_FLAGS "-Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")
endif ()

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_BINARY_DIR}/cMoonConfig.h"
  "${PROJECT_BINARY_DIR}/cMoonConfig.h.in"
  )

set (CMOON_SRCS
  src/cMoon.cpp
  src/AlgBase.cpp
  src/AObject.cpp
  src/ADateTime.cpp
  src/ALocation.cpp
  src/AMoon.cpp
  src/ASun.cpp
  src/APlanets.cpp
  src/settings.cpp
)

# add_library(cMoon_obj OBJECT ${CMOON_SRCS})

# add the binary tree to the search path for include files
# so that we will find cMoonConfig.h
# NOTE: source code includes are in the current source directory
include_directories("${PROJECT_BINARY_DIR}")

# add the executable
add_executable(cMoon ${CMOON_SRCS})
