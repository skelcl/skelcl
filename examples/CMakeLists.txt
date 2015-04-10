# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories (${SKELCL_COMMON_INCLUDE_DIR})
link_directories (${SKELCL_COMMON_LIB_DIR})

add_subdirectory (CannyEdgeDetection)
add_subdirectory (CannyEdgeDetection.fused)
add_subdirectory (DotProduct)
add_subdirectory (FDTD)
add_subdirectory (GameOfLife)
add_subdirectory (GaussianBlur)
add_subdirectory (HeatEquation)
add_subdirectory (MandelbrotSet)
add_subdirectory (MatrixMultiply)
add_subdirectory (PageRank)
add_subdirectory (SAXPY)

add_subdirectory (SimpleBig)
