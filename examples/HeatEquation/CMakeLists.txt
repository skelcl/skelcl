set (SKELCL_EXAMPLES_HEATEQUATION_SOURCES main.cpp)

add_executable (HeatEquation ${SKELCL_EXAMPLES_HEATEQUATION_SOURCES})
target_link_libraries (HeatEquation SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (MapOverlap.cl ${CMAKE_CURRENT_BINARY_DIR}/MapOverlap.cl)
configure_file (Stencil.cl ${CMAKE_CURRENT_BINARY_DIR}/Stencil.cl)

# directory where data is written to
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)
