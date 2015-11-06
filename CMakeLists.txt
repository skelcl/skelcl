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

# include custom cmake modules (e.g. FindOpenCL.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# project name
project (SkelCL)

# specify options and provide common variables
option (BUILD_SHARED_LIBS
        "Build shared libraries (DLLs)." ON)
option (BUILD_EXAMPLES
        "Build all examples automatically with the library." ON)
option (BUILD_EXECUTOR
        "Build executor tool." OFF)
option (BUILD_TESTS
        "Build all tests automatically with the library." ON)
option (BUILD_TOOLS
        "Build additional tools." ON)
option (THROW_ON_FAILURE
        "Throw an exception on failure instead of using std::exit." OFF)
option (BUILD_STOOLING
        "Build Stooling library. (required by SkelCL)" ON)

# add libraries
if (NOT TARGET pvsutil)
 add_subdirectory (libraries/pvsutil)
endif (NOT TARGET pvsutil)

if (BUILD_STOOLING)
  add_subdirectory (libraries/stooling)
endif (BUILD_STOOLING)

# opencl dirs
find_package (OpenCL REQUIRED)

# Add compiler and linker flags for OmniTune compatability.
# find_package (Glibmm REQUIRED)
# find_package (GIOmm REQUIRED)


# skelcl common dirs and libraries
set (SKELCL_COMMON_INCLUDE_DIR
      "${PROJECT_SOURCE_DIR}/include"
      "${PROJECT_SOURCE_DIR}/libraries/pvsutil/include"
      "${PROJECT_SOURCE_DIR}/libraries/stooling/include"
      # "${Glibmm_INCLUDE_DIRS}"
      # "${GIOmm_INCLUDE_DIRS}"
      #"${GObject_INCLUDE_DIRS}"
      "${STOOLING_COMMON_INCLUDE_DIR}" CACHE INTERNAL
     "Common include directories used by SkelCL.")
set (SKELCL_CORE_COMMON_LIBS
      ${OPENCL_LIBRARIES}
      crypto
      pvsutil CACHE INTERNAL
      "Common libraries used by the SkelCL core."
      )
if (BUILD_STOOLING)
  set (SKELCL_COMMON_LIBS
        ${SKELCL_CORE_COMMON_LIBS}
        stooling
        "${STOOLING_COMMON_LIBS}" CACHE INTERNAL
       "Common libraries used by SkelCL.")
else (BUILD_STOOLING)
  set (SKELCL_COMMON_LIBS
        ${SKELCL_CORE_COMMON_LIBS}
        "${STOOLING_COMMON_LIBS}" CACHE INTERNAL
       "Common libraries used by SkelCL.")

endif (BUILD_STOOLING)

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

if (THROW_ON_FAILURE)
  message(STATUS "Enabling exception on failure")
  add_definitions( -DTHROW_ON_FAILURE )
endif (THROW_ON_FAILURE)

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
