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
set (LLVM_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/libraries/llvm/include" CACHE STRING
     "Include directory of libclang.")
set (LLVM_LIB_DIR "${PROJECT_SOURCE_DIR}/libraries/llvm/lib" CACHE STRING
     "Library directory of libclang.")

execute_process(COMMAND "${LLVM_HOME_DIR}/bin/llvm-config" "--version"
                OUTPUT_VARIABLE LLVM_VERSION)
if ("${LLVM_VERSION}" STREQUAL "")
  message(FATAL_ERROR "No LLVM version found. Please make LLVM available at: ${LLVM_HOME_DIR}. (The file 'bin/llvm-config' and the folders 'include' and 'lib' are required.)")
else()
  message(STATUS "LLVM version found: ${LLVM_VERSION}")
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

# llvm version 3.5 or greater
if ("${LLVM_VERSION}" VERSION_GREATER "3.4.9")
set (STOOLING_COMMON_LIBS
      curses
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
      LLVMBitReader
      LLVMBitWriter
      LLVMTransformUtils
      LLVMipa
      LLVMAnalysis
      LLVMTarget
      LLVMMCParser
      LLVMMC
      LLVMOption
      LLVMObject
      LLVMCore
      LLVMSupport
      dl
      z
      CACHE INTERNAL
     "Common libraries used by stooling."
     )
elseif ("${LLVM_VERSION}" VERSION_GREATER "3.2.0")
set (STOOLING_COMMON_LIBS
      curses
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
      LLVMBitReader
      LLVMBitWriter
      LLVMTransformUtils
      LLVMipa
      LLVMAnalysis
      LLVMTarget
      LLVMMCParser
      LLVMMC
      LLVMOption
      LLVMObject
      LLVMCore
      LLVMSupport
      dl
      z
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

