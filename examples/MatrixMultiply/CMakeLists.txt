set (SKELCL_EXAMPLES_MATRIXMULTIPLY_SOURCES main.cpp)

add_executable (MatrixMultiply ${SKELCL_EXAMPLES_MATRIXMULTIPLY_SOURCES})
target_link_libraries (MatrixMultiply SkelCL ${SKELCL_COMMON_LIBS})
