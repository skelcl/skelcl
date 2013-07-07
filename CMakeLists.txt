cmake_minimum_required (VERSION 2.6)

# project name
project (SkelCL)

# set compiler and options
if (${UNIX})
  if (${APPLE})
    set (CMAKE_CXX_COMPILER "clang++" CACHE STRING "CXX Compiler.")
    set (CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -DGTEST_USE_OWN_TR1_TUPLE=1 -Wall -Wextra"
      CACHE STRING "Flags used by the compiler during all build types.")
  else (${APPLE})
    set (CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra"
      CACHE STRING "Flags used by the compiler during all build types.")
  endif (${APPLE})

  # more restructive flags for compiling the SkelCL code. These flags are added in addition to the above
  set (SKELCL_CXX_FLAGS "-Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Werror -pedantic-errors -Wfatal-errors"
      CACHE STRING "More strict flags used to compile the SkelCL sources.")
endif (${UNIX})

# set default build type explicitly to Debug
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

# specify options and provide common variables
option (BUILD_SHARED_LIBS
        "Build shared libraries (DLLs)." ON)
option (BUILD_EXAMPLES
        "Build all examples automatically with the library." ON)
option (BUILD_TESTS
        "Build all tests automatically with the library." ON)

# set common output directory
if (NOT EXECUTABLE_OUTPUT_PATH)
  set (EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
endif (NOT EXECUTABLE_OUTPUT_PATH)

if (NOT LIBRARY_OUTPUT_PATH)
  set (LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
endif (NOT LIBRARY_OUTPUT_PATH)

# include custom cmake modules (e.g. FindOpenCL.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
# opencl dirs
find_package (OpenCL)

# skelcl common dirs and libraries
set (SKELCL_COMMON_INCLUDE_DIR
      "${PROJECT_SOURCE_DIR}/include"
      "${OPENCL_INCLUDE_DIRS}"
      "${PVSUTIL_COMMON_INCLUDE_DIR}"
      "${STOOLING_COMMON_INCLUDE_DIR}" 
     CACHE INTERNAL
     "Common include directories used by SkelCL.")

set (SKELCL_COMMON_LIBS
      ${OPENCL_LIBRARIES}
      crypto
      pvsutil
      stooling
      "${PVSUTIL_COMMON_LIBS}"
      "${STOOLING_COMMON_LIBS}" 
     CACHE INTERNAL
     "Common libraries used by SkelCL.")

set (SKELCL_COMMON_LIB_DIR
      "${OPENCL_LIB_DIR}"
      "${PVSUTIL_COMMON_LIB_DIR}"
      "${STOOLING_COMMON_LIB_DIR}"
     CACHE INTERNAL
     "Common library directories used by SkelCL.")

#build tests
if (BUILD_TESTS)
  # this if prevents gtest from being build multiple times
  if (NOT TARGET gtest)
    add_subdirectory (libraries/gtest)
    set (GTEST_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/libraries/gtest/include")
  endif (NOT TARGET gtest)

  enable_testing ()
  add_subdirectory (test)
endif (BUILD_TESTS)

# add libraries
if (NOT TARGET pvsutil)
  add_subdirectory (libraries/pvsutil)
endif (NOT TARGET pvsutil)

if (NOT TARGET stooling)
 add_subdirectory (libraries/stooling)
endif (NOT TARGET stooling)

# add subdirectory to build library
add_subdirectory (src)

# build examples
if (BUILD_EXAMPLES)
  add_subdirectory (examples)
endif (BUILD_EXAMPLES)

