set (SKELCL_EXAMPLES_GAUSS_SOURCES
      main.cpp
    )

add_executable (gauss ${SKELCL_EXAMPLES_GAUSS_SOURCES})
target_link_libraries (gauss SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (gauss2D.cl ${CMAKE_CURRENT_BINARY_DIR}/gauss2D.cl)



