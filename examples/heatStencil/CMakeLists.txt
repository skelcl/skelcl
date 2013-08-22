set (SKELCL_EXAMPLES_HEATSTENCIL_SOURCES
      main.cpp
    )

add_executable (heatStencil ${SKELCL_EXAMPLES_HEATSTENCIL_SOURCES})
target_link_libraries (heatStencil SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (heat.cl ${CMAKE_CURRENT_BINARY_DIR}/heat.cl)



