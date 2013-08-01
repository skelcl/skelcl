set (SKELCL_EXAMPLES_MANDELBROT_SOURCES
      main.cpp
    )

add_executable (mandelbrot ${SKELCL_EXAMPLES_MANDELBROT_SOURCES})
target_link_libraries (mandelbrot SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (mandelbrot.cl ${CMAKE_CURRENT_BINARY_DIR}/mandelbrot.cl)

