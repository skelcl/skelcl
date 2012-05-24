set (SKELCL_EXAMPLES_DOT_PRODUCT_SOURCES
      main.cpp
    )

  add_executable (dot_product ${SKELCL_EXAMPLES_DOT_PRODUCT_SOURCES})
target_link_libraries (dot_product SkelCL ${SKELCL_COMMON_LIBS})

