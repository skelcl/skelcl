set (SKELCL_EXAMPLES_SAXPY_SOURCES
      main.cpp
    )

add_executable (montecarlo ${SKELCL_EXAMPLES_SAXPY_SOURCES})
target_link_libraries (montecarlo SkelCL ${SKELCL_COMMON_LIBS})
