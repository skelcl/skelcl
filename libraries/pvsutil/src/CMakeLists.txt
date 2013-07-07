set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PVSUTIL_CXX_FLAGS}")

# set include dir
include_directories (${PVSUTIL_COMMON_INCLUDE_DIR})

link_directories (${PVSUTIL_COMMON_LIB_DIR})

# set files used to build library
set (PVSUTIL_SOURCES
      ../include/pvsutil/Assert.h
      ../include/pvsutil/Logger.h
      ../include/pvsutil/CLArgParser.h
      ../include/pvsutil/Timer.h
      ../include/pvsutil/cmdline/Arg.h
      ../include/pvsutil/cmdline/Default.h
      ../include/pvsutil/cmdline/Description.h
      ../include/pvsutil/cmdline/Flags.h
      ../include/pvsutil/cmdline/Version.h
      ../include/pvsutil/detail/LoggerDef.h
      ../include/pvsutil/detail/pvsutilDll.h
      Arg.cpp
      Assert.cpp
      Description.cpp
      Flags.cpp
      Logger.cpp
      CLArgParser.cpp
      Timer.cpp
      Version.cpp
    )

# specify library target
add_library (pvsutil ${PVSUTIL_SOURCES})
target_link_libraries( pvsutil ${PVSUTIL_COMMON_LIBS})

# define DLL export Macro
if (WIN32)
  set_target_properties(pvsutil PROPERTIES DEFINE_SYMBOL PVSUTIL_EXPORTS)
endif (WIN32)

