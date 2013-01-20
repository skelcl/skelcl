set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PVSUTIL_CXX_FLAGS}")

# set include dir
include_directories (
  "${PROJECT_SOURCE_DIR}/include"
                    )

# set files used to build library
set (PVSUTIL_SOURCES
      ../include/pvsutil/Assert.h
      ../include/pvsutil/Logger.h
      ../include/pvsutil/detail/LoggerDef.h
      Assert.cpp
      Logger.cpp
    )

# specify library target
add_library (pvsutil ${PVSUTIL_SOURCES})

# specify install
if (OS_LINUX)
	# specify install into directory DESTINATION/lib
  install (TARGETS pvsutil
					 LIBRARY DESTINATION lib
					 ARCHIVE DESTINATION lib
					)
endif (OS_LINUX)

