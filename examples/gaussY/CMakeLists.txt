set (SKELCL_EXAMPLES_GAUSSY_SOURCES
      main.cpp
    )

add_executable (gaussY ${SKELCL_EXAMPLES_GAUSSY_SOURCES})
target_link_libraries (gaussY SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (gaussY.cl ${CMAKE_CURRENT_BINARY_DIR}/gaussY.cl)
configure_file (data/lena.pgm ${CMAKE_CURRENT_BINARY_DIR}/data/lena.pgm)
