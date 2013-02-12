set (SKELCL_EXAMPLES_MATRIX_MULT_SOURCES
      main.cpp
    )

add_executable (matrix_mult ${SKELCL_EXAMPLES_MATRIX_MULT_SOURCES})
target_link_libraries (matrix_mult SkelCL ${SKELCL_COMMON_LIBS})

