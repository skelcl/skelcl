set (SKELCL_EXAMPLES_CANNY_SOURCES
      main.cpp
    )

add_executable (CannyEdgeDetection ${SKELCL_EXAMPLES_CANNY_SOURCES})
target_link_libraries (CannyEdgeDetection SkelCL ${SKELCL_COMMON_LIBS})

configure_file (MapOverlapGauss.cl ${CMAKE_CURRENT_BINARY_DIR}/MapOverlapGauss.cl)
configure_file (MapOverlapNMS.cl ${CMAKE_CURRENT_BINARY_DIR}/MapOverlapNMS.cl)
configure_file (MapOverlapSobel.cl ${CMAKE_CURRENT_BINARY_DIR}/MapOverlapSobel.cl)
configure_file (MapOverlapThreshold.cl ${CMAKE_CURRENT_BINARY_DIR}/MapOverlapThreshold.cl)
configure_file (StencilGauss.cl ${CMAKE_CURRENT_BINARY_DIR}/StencilGauss.cl)
configure_file (StencilNMS.cl ${CMAKE_CURRENT_BINARY_DIR}/StencilNMS.cl)
configure_file (StencilSobel.cl ${CMAKE_CURRENT_BINARY_DIR}/StencilSobel.cl)
configure_file (StencilThreshold.cl ${CMAKE_CURRENT_BINARY_DIR}/StencilThreshold.cl)
configure_file (data/lena.pgm ${CMAKE_CURRENT_BINARY_DIR}/data/lena.pgm)
