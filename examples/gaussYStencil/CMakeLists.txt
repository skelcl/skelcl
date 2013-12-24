set (SKELCL_EXAMPLES_GAUSSYSTENCIL_SOURCES
      main.cpp
    )

add_executable (gaussYStencil ${SKELCL_EXAMPLES_GAUSSYSTENCIL_SOURCES})
target_link_libraries (gaussYStencil SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (gaussY.cl ${CMAKE_CURRENT_BINARY_DIR}/gaussY.cl)



