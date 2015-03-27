set (SKELCL_EXAMPLES_CANNY_SOURCES main.cpp)

add_executable (CannyEdgeDetection.fused ${SKELCL_EXAMPLES_CANNY_SOURCES})
target_link_libraries (CannyEdgeDetection.fused SkelCL ${SKELCL_COMMON_LIBS})

configure_file (kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl)
configure_file (data/lena.pgm ${CMAKE_CURRENT_BINARY_DIR}/data/lena.pgm)
