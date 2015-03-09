set (SKELCL_EXAMPLES_GAUSSIANBLUR_SOURCES main.cpp )

add_executable (GaussianBlur ${SKELCL_EXAMPLES_GAUSSIANBLUR_SOURCES})
target_link_libraries (GaussianBlur SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (Stencil1D.cl ${CMAKE_CURRENT_BINARY_DIR}/Stencil1D.cl)
configure_file (Stencil2D.cl ${CMAKE_CURRENT_BINARY_DIR}/Stencil2D.cl)
configure_file (MapOverlap1D.cl ${CMAKE_CURRENT_BINARY_DIR}/MapOverlap1D.cl)
configure_file (MapOverlap2D.cl ${CMAKE_CURRENT_BINARY_DIR}/MapOverlap2D.cl)
configure_file (data/lena.pgm ${CMAKE_CURRENT_BINARY_DIR}/data/lena.pgm)
