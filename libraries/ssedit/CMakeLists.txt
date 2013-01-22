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
  set (SSE_CXX_FLAGS "-Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Werror -pedantic-errors -Wfatal-errors"
      CACHE STRING "More strict flags used to compile the SSE sources.")
endif (${UNIX})

# project name
project (ssedit)

# specify options and provide common variables
option (BUILD_SHARED_LIBS
        "Build shared libraries (DLLs)." ON)
option (BUILD_TESTS
        "Build all tests automatically with the library." ON)

# libclang dirs
set (LIBCLANG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/libraries/libclang/include" CACHE STRING
     "Include directory of libclang.")
set (LIBCLANG_LIB_DIR "${PROJECT_SOURCE_DIR}/libraries/libclang/lib" CACHE STRING
     "Library directory of libclang.")

# add libraries
if (NOT TARGET pvsutil)
  add_subdirectory (libraries/pvsutil)
endif (NOT TARGET pvsutil)

# skelcl's ssedit common dirs and libraries
set (SSEDIT_COMMON_INCLUDE_DIR
      "${PROJECT_SOURCE_DIR}/include"
      "${PROJECT_SOURCE_DIR}/libraries/pvsutil/include"
      ${LIBCLANG_INCLUDE_DIR} CACHE INTERNAL
     "Common include directories used by ssedit.")
set (SSEDIT_COMMON_LIBS
      pvsutil
      clang CACHE INTERNAL
     "Common libraries used by ssedit.")
set (SSEDIT_COMMON_LIB_DIR
      "${PROJECT_BINARY_DIR}/libraries/pvsutil/lib"
      ${LIBCLANG_LIB_DIR} CACHE INTERNAL
     "Common library directories used by ssedit.")

# set default build type explicitly to Debug
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

# add subdirectory to build library
add_subdirectory (src)

# add subdirectory to build tools
add_subdirectory (tools)

# build tests
if (BUILD_TESTS)
  # this if prevents gtest from being build multiple times
  if (NOT TARGET gtest)
    add_subdirectory (libraries/gtest)
  endif (NOT TARGET gtest)

	enable_testing ()
  add_subdirectory (test)
endif (BUILD_TESTS)

