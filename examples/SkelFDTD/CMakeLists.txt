set (SKELCL_EXAMPLES_MANDELBROT_SOURCES
      SkelFDTD.cpp
      Simulation.cpp
    )

add_executable (SkelFDTD ${SKELCL_EXAMPLES_MANDELBROT_SOURCES})
target_link_libraries (SkelFDTD SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (Stencils.cl ${CMAKE_CURRENT_BINARY_DIR}/Stencils.cl)
configure_file (Kernels.cl ${CMAKE_CURRENT_BINARY_DIR}/Kernels.cl)
