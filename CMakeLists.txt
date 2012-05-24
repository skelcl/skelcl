cmake_minimum_required (VERSION 2.6)

if (${WIN32})
  message ("Build on Windows not (yet) supported.")
endif (${WIN32})

# set compiler and options
if (${UNIX})
  # assume these flags are valid for any unix compiler (they are at least for g++ and clang++)
  set (CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra"
		  CACHE STRING "Flags used by the compiler during all build types.")

  # more restructive flags for compiling the SkelCL code. These flags are added in addition to the above
  set (SKELCL_CXX_FLAGS "-Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Werror -pedantic-errors -Wfatal-errors"
      CACHE STRING "More strict flags used to compile the SSE sources.")
endif (${UNIX})

# project name
project (SkelCL)

# add libraries
add_subdirectory (libraries/ssedit)

# specify options and provide common variables
option (BUILD_SHARED_LIBS
        "Build shared libraries (DLLs)." ON)
option (BUILD_EXAMPLES
        "Build all examples automatically with the library." ON)
option (BUILD_TESTS
        "Build all tests automatically with the library." ON)

# opencl dirs
set (OPENCL_INCLUDE_DIR /usr/include CACHE STRING
     "OpenCL include directory.")
set (OPENCL_LIB_DIR /usr/lib CACHE STRING
     "OpenCL library directory.")

# skelcl common dirs and libraries
set (SKELCL_COMMON_INCLUDE_DIR
      "${OPENCL_INCLUDE_DIR}"
      "${PROJECT_SOURCE_DIR}/libraries/ssedit/include"
      "${SSEDIT_COMMON_INCLUDE_DIR}" CACHE INTERNAL
     "Common include directories used by SkelCL.")
set (SKELCL_COMMON_LIBS
      OpenCL
      crypto
      ssedit
      "${SSEDIT_COMMON_LIBS}" CACHE INTERNAL
     "Common libraries used by SkelCL.")
set (SKELCL_COMMON_LIB_DIR
      "${OPENCL_LIB_DIR}"
      "${PROJECT_BINARY_DIR}/libraries/ssedit/lib"
      "${SSEDIT_COMMON_LIB_DIR}" CACHE INTERNAL
     "Common library directories used by SkelCL.")

# set default build type explicitly to Debug
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

# add subdirectory to build library
add_subdirectory (src)

# build examples
if (BUILD_EXAMPLES)
  add_subdirectory (examples)
endif (BUILD_EXAMPLES)

#build tests
if (BUILD_TESTS)
  # this if prevents gtest from being build multiple times
  if (NOT TARGET gtest)
    add_subdirectory (libraries/gtest)
  endif (NOT TARGET gtest)

  enable_testing ()
  add_subdirectory (test)
endif (BUILD_TESTS)

