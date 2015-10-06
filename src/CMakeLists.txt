set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKELCL_CXX_FLAGS}")

# set include dir
include_directories (${SKELCL_COMMON_INCLUDE_DIR})

link_directories (${SKELCL_COMMON_LIB_DIR})

# set files used to build library

set (SKELCL_CORE_SOURCES
      Device.cpp
      DeviceBuffer.cpp
      DeviceID.cpp
      DeviceList.cpp
      DeviceProperties.cpp
      Event.cpp
      KernelUtil.cpp
      Local.cpp
      PlatformID.cpp
      SkelCL.cpp
      Source.cpp
      Util.cpp
      )

add_library (SkelCLCore ${SKELCL_CORE_SOURCES})
target_link_libraries (SkelCLCore ${SKELCL_CORE_COMMON_LIBS})

set (SKELCL_SOURCES
      Index.cpp
      IndexMatrix.cpp
      IndexVector.cpp
      Map.cpp
      MatrixSize.cpp
      Program.cpp
      Skeleton.cpp
      Significances.cpp
    )

set (SKELCL_HEADERS
      ../include/SkelCL/AllPairs.h
      ../include/SkelCL/Distributions.h
      ../include/SkelCL/Index.h
      ../include/SkelCL/IndexMatrix.h
      ../include/SkelCL/IndexVector.h
      ../include/SkelCL/Local.h
      ../include/SkelCL/Map.h
      ../include/SkelCL/MapOverlap.h
      ../include/SkelCL/Matrix.h
      ../include/SkelCL/Out.h
      ../include/SkelCL/Reduce.h
      ../include/SkelCL/Scan.h
      ../include/SkelCL/SkelCL.h
      ../include/SkelCL/Source.h
      ../include/SkelCL/Vector.h
      ../include/SkelCL/Zip.h
      ../include/SkelCL/detail/AllPairsDef.h
      ../include/SkelCL/detail/AllPairsKernel.cl
      ../include/SkelCL/detail/AllPairsKernel2.cl
      ../include/SkelCL/detail/AllPairsKernel3.cl
      ../include/SkelCL/detail/BlockDistribution.h
      ../include/SkelCL/detail/BlockDistributionDef.h
      ../include/SkelCL/detail/Container.h
      ../include/SkelCL/detail/CopyDistribution.h
      ../include/SkelCL/detail/CopyDistributionDef.h
      ../include/SkelCL/detail/Device.h
      ../include/SkelCL/detail/DeviceBuffer.h
      ../include/SkelCL/detail/DeviceID.h
      ../include/SkelCL/detail/DeviceList.h
      ../include/SkelCL/detail/DeviceProperties.h
      ../include/SkelCL/detail/Distribution.h
      ../include/SkelCL/detail/DistributionDef.h
      ../include/SkelCL/detail/Event.h
      ../include/SkelCL/detail/IndexMatrixDef.h
      ../include/SkelCL/detail/IndexVectorDef.h
      ../include/SkelCL/detail/KernelUtil.h
      ../include/SkelCL/detail/Macros.h
      ../include/SkelCL/detail/MapDef.h
      ../include/SkelCL/detail/MapHelper.h
      ../include/SkelCL/detail/MapHelperDef.h
      ../include/SkelCL/detail/MapOverlapDef.h
      ../include/SkelCL/detail/MapOverlapKernel.cl
      ../include/SkelCL/detail/MatrixDef.h
      ../include/SkelCL/detail/OLDistribution.h
      ../include/SkelCL/detail/OLDistributionDef.h
      ../include/SkelCL/detail/OverlapDistribution.h
      ../include/SkelCL/detail/OverlapDistributionDef.h
      ../include/SkelCL/detail/Padding.h
      ../include/SkelCL/detail/PlatformID.h
      ../include/SkelCL/detail/Program.h
      ../include/SkelCL/detail/ReduceDef.h
      ../include/SkelCL/detail/ReduceKernel.cl
      ../include/SkelCL/detail/ScanDef.h
      ../include/SkelCL/detail/ScanKernel.cl
      ../include/SkelCL/detail/Significances.h
      ../include/SkelCL/detail/SingleDistribution.h
      ../include/SkelCL/detail/SingleDistributionDef.h
      ../include/SkelCL/detail/skelclDll.h
      ../include/SkelCL/detail/Skeleton.h
      ../include/SkelCL/detail/Types.h
      ../include/SkelCL/detail/Util.h
      ../include/SkelCL/detail/VectorDef.h
      ../include/SkelCL/detail/ZipDef.h
    )

# specify library target
add_library (SkelCL ${SKELCL_SOURCES} ${SKELCL_HEADERS})
target_link_libraries (SkelCL SkelCLCore ${SKELCL_COMMON_LIBS})

install (TARGETS SkelCL
         LIBRARY DESTINATION lib)

install (DIRECTORY ../include/SkelCL
         DESTINATION include)

