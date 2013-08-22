set (SKELCL_EXAMPLES_HEAT_SOURCES
      main.cpp
    )

add_executable (heat ${SKELCL_EXAMPLES_HEAT_SOURCES})
target_link_libraries (heat SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (heat.cl ${CMAKE_CURRENT_BINARY_DIR}/heat.cl)



