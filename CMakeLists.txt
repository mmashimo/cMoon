
cmake_minimum_required (VERSION 3.0)
project (cMoon)

# The version number.
set (cMoon_VERSION_MAJOR 1)
set (cMoon_VERSION_MINOR 0)

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
  "${PROJECT_SOURCE_DIR}/cMoonConfig.h.in"
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
)

# add_library(cMoon_obj OBJECT ${CMOON_SRCS})

# add the binary tree to the search path for include files
# so that we will find cMoonConfig.h (${PROJECT_BINARY_DIR})
# NOTE: source code includes are in the current source directory
include_directories("${PROJECT_BINARY_DIR}")

# add the executable
add_executable(cMoon ${CMOON_SRCS})
