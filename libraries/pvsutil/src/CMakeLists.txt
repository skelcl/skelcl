set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PVSUTIL_CXX_FLAGS}")

# set include dir
include_directories (
  "${PROJECT_SOURCE_DIR}/include"
                    )

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

install (TARGETS pvsutil
         LIBRARY DESTINATION lib)

install (DIRECTORY ../include/pvsutil
         DESTINATION include)

