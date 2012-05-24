set (SKELCL_EXAMPLES_SAXPY_SOURCES
      main.cpp
    )

add_executable (saxpy ${SKELCL_EXAMPLES_SAXPY_SOURCES})
target_link_libraries (saxpy SkelCL ${SKELCL_COMMON_LIBS})

