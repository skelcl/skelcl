set (SKELCL_EXAMPLES_GAUSSSTENCIL_SOURCES
      main.cpp
    )

add_executable (gaussStencil ${SKELCL_EXAMPLES_GAUSSSTENCIL_SOURCES})
target_link_libraries (gaussStencil SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (gauss2DStencil.cl ${CMAKE_CURRENT_BINARY_DIR}/gauss2DStencil.cl)
configure_file (pgm.pgm ${CMAKE_CURRENT_BINARY_DIR}/pgm.pgm)



