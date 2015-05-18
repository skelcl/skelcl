cmake_minimum_required (VERSION 2.6)

if (${WIN32})
  message ("Build on Windows not (yet) supported.")
endif (${WIN32})

# set compiler and options
if (${UNIX})
  if (${APPLE})
    set (CMAKE_CXX_COMPILER "clang++" CACHE STRING "CXX Compiler.")
    set (CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -DGTEST_USE_OWN_TR1_TUPLE=1 -Wall -Wextra"
      CACHE STRING "Flags used by the compiler during all build types.")
  else (${APPLE})
    set (CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -pthread"
      CACHE STRING "Flags used by the compiler during all build types.")
  endif (${APPLE})

  # more restructive flags for compiling the SkelCL code. These flags are added in addition to the above
  set (SKELCL_CXX_FLAGS "-Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Werror -pedantic-errors -Wfatal-errors"
      CACHE STRING "More strict flags used to compile the SkelCL sources.")
endif (${UNIX})

# Add compiler and linker flags for OmniTune compatability.
# FIXME: Use the CMake equivalent of `pkg-config --cflags glibmm-2.4 giomm-2.4` and `pkg-config --libs glibmm-2.4 giomm-2.4` for portability.
set (OMNITUNE_CXX_FLAGS "-pthread -I/usr/include/giomm-2.4 -I/usr/lib64/giomm-2.4/include -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include"
     CACHE STRING "Compiler flags for OmniTune compatability.")
set (OMNITUNE_LD_FLAGS "-lgiomm-2.4 -lgio-2.0 -lglibmm-2.4 -lgobject-2.0 -lglib-2.0 -lsigc-2.0"
     CACHE STRING "Compiler flags for OmniTune compatability.")
add_definitions(${OMNITUNE_CXX_FLAGS})
add_definitions(${OMNITUNE_LD_FLAGS})
set (CMAKE_SHARED_LINKER_FLAGS "${OMNITUNE_LD_FLAGS}")

# include custom cmake modules (e.g. FindOpenCL.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# project name
project (SkelCL)

# add libraries
if (NOT TARGET pvsutil)
 add_subdirectory (libraries/pvsutil)
endif (NOT TARGET pvsutil)
add_subdirectory (libraries/stooling)

# specify options and provide common variables
option (BUILD_SHARED_LIBS
        "Build shared libraries (DLLs)." ON)
option (BUILD_EXAMPLES
        "Build all examples automatically with the library." ON)
option (BUILD_EXECUTOR
        "Build executor tool." ON)
option (BUILD_TESTS
        "Build all tests automatically with the library." ON)
option (BUILD_TOOLS
        "Build additional tools." ON)

# opencl dirs
find_package (OpenCL)

# skelcl common dirs and libraries
set (SKELCL_COMMON_INCLUDE_DIR
      "${PROJECT_SOURCE_DIR}/include"
      "${PROJECT_SOURCE_DIR}/libraries/pvsutil/include"
      "${PROJECT_SOURCE_DIR}/libraries/stooling/include"
      "${STOOLING_COMMON_INCLUDE_DIR}" CACHE INTERNAL
     "Common include directories used by SkelCL.")
set (SKELCL_COMMON_LIBS
      ${OPENCL_LIBRARIES}
      crypto
      pvsutil
      stooling
      "${STOOLING_COMMON_LIBS}" CACHE INTERNAL
     "Common libraries used by SkelCL.")
set (SKELCL_COMMON_LIB_DIR
      "${OPENCL_LIB_DIR}"
      "${PROJECT_BINARY_DIR}/libraries/pvsutil/lib"
      "${PROJECT_BINARY_DIR}/libraries/stooling/lib"
      "${STOOLING_COMMON_LIB_DIR}" CACHE INTERNAL
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

# build executor
if (BUILD_EXECUTOR)
  add_subdirectory (executor)
endif (BUILD_EXECUTOR)

#build tests
if (BUILD_TESTS)
  # this if prevents gtest from being build multiple times
  if (NOT TARGET gtest)
    add_subdirectory (libraries/gtest)
  endif (NOT TARGET gtest)

  enable_testing ()
  add_subdirectory (test)
endif (BUILD_TESTS)

# build tools
if (BUILD_TOOLS)
  add_subdirectory (tools)
endif (BUILD_TOOLS)
