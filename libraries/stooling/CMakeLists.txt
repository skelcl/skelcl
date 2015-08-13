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
    set (CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra"
      CACHE STRING "Flags used by the compiler during all build types.")
  endif (${APPLE})

  # more restructive flags for compiling the SkelCL code. These flags are added in addition to the above
  set (STOOLING_CXX_FLAGS "-fno-rtti -Wall -Wextra -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Werror -pedantic-errors -Wfatal-errors -Wno-non-virtual-dtor"
    CACHE STRING "More strict flags used to compile the STOOLING sources.")
endif (${UNIX})

# project name
project (stooling)

# specify options and provide common variables
option (BUILD_SHARED_LIBS
  "Build shared libraries (DLLs)." ON)
option (BUILD_TESTS
        "Build all tests automatically with the library." ON)

# libclang dirs
set (LLVM_HOME_DIR "${PROJECT_SOURCE_DIR}/libraries/llvm" CACHE STRING
     "Home directory of libclang and LLVM.")

# determine which llvm-config to use
if (EXISTS "${LLVM_HOME_DIR}/bin/llvm-config")
  set (LLVM_CONFIG "${LLVM_HOME_DIR}/bin/llvm-config" CACHE STRING "llvm-config executable")
  message(STATUS "Use local version of LLVM in ${LLVM_HOME_DIR}")
else()
  execute_process(COMMAND "llvm-config" "--version"
                   RESULT_VARIABLE SYSTEM_LLVM_CONFIG)
  if ("${SYSTEM_LLVM_CONFIG}" STREQUAL "0")
    set (LLVM_CONFIG "llvm-config" CACHE STRING "llvm-config executable")
    message(STATUS "Use system wide version of LLVM")
  else()
    message(FATAL_ERROR "No LLVM version found. Please make LLVM available at: ${LLVM_HOME_DIR}. (The file 'bin/llvm-config' and the folders 'include' and 'lib' are required.)")
  endif()
endif()

execute_process(COMMAND "${LLVM_CONFIG}" "--version"
                OUTPUT_VARIABLE LLVM_VERSION)
message(STATUS "LLVM version found: ${LLVM_VERSION}")

execute_process(COMMAND "${LLVM_CONFIG}" "--includedir"
               OUTPUT_VARIABLE LLVM_INCLUDE_DIR
               OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "${LLVM_CONFIG}" "--libdir"
               OUTPUT_VARIABLE LLVM_LIB_DIR
               OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND "${LLVM_CONFIG}" "--cflags" "--cxxflags"
               OUTPUT_VARIABLE LLVM_CXX_FLAGS_
               OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\n" " " LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS_})
#set(STOOLING_CXX_FLAGS "${STOOLING_CXX_FLAGS} ${LLVM_CXX_FLAGS}")

if ("${LLVM_VERSION}" VERSION_GREATER "3.5.9")
execute_process(COMMAND "${LLVM_CONFIG}" "--ldflags" "--libs" "--system-libs"
               OUTPUT_VARIABLE LLVM_LINKER_FLAGS_
               OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\n" " " LLVM_LINKER_FLAGS ${LLVM_LINKER_FLAGS_})
else ()
execute_process(COMMAND "${LLVM_CONFIG}" "--ldflags" "--libs"
               OUTPUT_VARIABLE LLVM_LINKER_FLAGS_
               OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\n" " " LLVM_LINKER_FLAGS ${LLVM_LINKER_FLAGS_})
endif()

# add libraries
if (NOT TARGET pvsutil)
  add_subdirectory (libraries/pvsutil)
endif (NOT TARGET pvsutil)

# skelcl's stooling common dirs and libraries
set (STOOLING_COMMON_INCLUDE_DIR
      "${PROJECT_SOURCE_DIR}/include"
      "${PROJECT_SOURCE_DIR}/libraries/pvsutil/include"
      "${LLVM_INCLUDE_DIR}" CACHE INTERNAL
     "Common include directories used by stooling.")

# llvm version 3.6 or greater
if ("${LLVM_VERSION}" VERSION_GREATER "3.5.9")
set (STOOLING_COMMON_LIBS
      clang
      clangTooling
      clangToolingCore
      clangRewrite
      clangRewriteFrontend
      clangFrontendTool
      clangFrontend
      clangParse
      clangSema
      clangAnalysis
      clangDriver
      clangStaticAnalyzerCheckers
      clangStaticAnalyzerFrontend
      clangStaticAnalyzerCore
      clangEdit
      clangFormat
      clangSerialization
      clangASTMatchers
      clangAST
      clangLex
      clangBasic
      CACHE INTERNAL
     "Common libraries used by stooling."
     )
# llvm version 3.5 or greater
elseif ("${LLVM_VERSION}" VERSION_GREATER "3.4.9")
set (STOOLING_COMMON_LIBS
      clang
      clangTooling
      clangRewrite
      clangRewriteFrontend
      clangFrontendTool
      clangFrontend
      clangParse
      clangSema
      clangAnalysis
      clangDriver
      clangStaticAnalyzerCheckers
      clangStaticAnalyzerFrontend
      clangStaticAnalyzerCore
      clangEdit
      clangFormat
      clangSerialization
      clangASTMatchers
      clangAST
      clangLex
      clangBasic
      CACHE INTERNAL
     "Common libraries used by stooling."
     )
elseif ("${LLVM_VERSION}" VERSION_GREATER "3.2.0")
set (STOOLING_COMMON_LIBS
      clang
      clangTooling
      clangRewriteCore
      clangRewriteFrontend
      clangFrontendTool
      clangFrontend
      clangParse
      clangSema
      clangAnalysis
      clangDriver
      clangStaticAnalyzerCheckers
      clangStaticAnalyzerFrontend
      clangStaticAnalyzerCore
      clangEdit
      clangFormat
      clangSerialization
      clangASTMatchers
      clangAST
      clangLex
      clangBasic
      CACHE INTERNAL
     "Common libraries used by stooling."
     )
endif()

set (STOOLING_COMMON_LIB_DIR
      "${LLVM_LIB_DIR}" CACHE INTERNAL
     "Common library directories used by stooling.")

# set default build type explicitly to Debug
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

# add subdirectory to build library
add_subdirectory (src)

# build tests
if (BUILD_TESTS)
  # this if prevents gtest from being build multiple times
  if (NOT TARGET gtest)
    add_subdirectory (libraries/gtest)
  endif (NOT TARGET gtest)

  enable_testing ()
  add_subdirectory (test)
endif (BUILD_TESTS)

