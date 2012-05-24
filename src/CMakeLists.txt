set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKELCL_CXX_FLAGS}")

# set include dir
include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories (${SKELCL_COMMON_INCLUDE_DIR})

link_directories (${SKELCL_COMMON_LIB_DIR})

# set files used to build library
set (SKELCL_SOURCES
      Assert.cpp
      BlockDistribution.cpp
      CopyDistribution.cpp
      Device.cpp
      DeviceBuffer.cpp
      DeviceList.cpp
      Distribution.cpp
      Event.cpp
      SkelCL.cpp
      Logger.cpp
      Program.cpp
      Significances.cpp
      SingleDistribution.cpp
      Skeleton.cpp
      Source.cpp
      Util.cpp
    )

set (SKELCL_HEADERS
      ../include/SkelCL/Distribution.h
      ../include/SkelCL/SkelCL.h
      ../include/SkelCL/Map.h
      ../include/SkelCL/Out.h
      ../include/SkelCL/Reduce.h
      ../include/SkelCL/Source.h
      ../include/SkelCL/Vector.h
      ../include/SkelCL/Zip.h
      ../include/SkelCL/detail/Assert.h
      ../include/SkelCL/detail/BlockDistribution.h
      ../include/SkelCL/detail/Container.h
      ../include/SkelCL/detail/CopyDistribution.h
      ../include/SkelCL/detail/CopyDistributionDef.h
      ../include/SkelCL/detail/Device.h
      ../include/SkelCL/detail/DeviceBuffer.h
      ../include/SkelCL/detail/DeviceList.h
      ../include/SkelCL/detail/Event.h
      ../include/SkelCL/detail/KernelUtil.h
      ../include/SkelCL/detail/Logger.h
      ../include/SkelCL/detail/LoggerDef.h
      ../include/SkelCL/detail/MapDef.h
      ../include/SkelCL/detail/MapHelper.h
      ../include/SkelCL/detail/MapHelperDef.h
      ../include/SkelCL/detail/Program.h
      ../include/SkelCL/detail/ReduceDef.h
      ../include/SkelCL/detail/ReduceKernel.cl
      ../include/SkelCL/detail/Significances.h
      ../include/SkelCL/detail/SingleDistribution.h
      ../include/SkelCL/detail/Skeleton.h
      ../include/SkelCL/detail/Util.h
      ../include/SkelCL/detail/VectorDef.h
      ../include/SkelCL/detail/ZipDef.h
    )

# specify library target
add_library (SkelCL ${SKELCL_SOURCES} ${SKELCL_HEADERS})
target_link_libraries (SkelCL ${SKELCL_COMMON_LIBS})

# specify install
if (OS_LINUX)
	# specify install into directory DESTINATION/lib
  install (TARGETS SkelCL
					 LIBRARY DESTINATION lib
					 ARCHIVE DESTINATION lib
					)
endif (OS_LINUX)

