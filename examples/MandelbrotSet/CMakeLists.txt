set (SKELCL_EXAMPLES_MANDELBROTSET_SOURCES main.cpp)

add_executable (MandelbrotSet ${SKELCL_EXAMPLES_MANDELBROTSET_SOURCES})
target_link_libraries (MandelbrotSet SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (mandelbrot.cl ${CMAKE_CURRENT_BINARY_DIR}/mandelbrot.cl)

# directory where data is written to
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)
