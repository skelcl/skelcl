set (SKELCL_EXAMPLES_GAMEOFLIFE_SOURCES main.cpp)

add_executable (GameOfLife ${SKELCL_EXAMPLES_GAMEOFLIFE_SOURCES})
target_link_libraries (GameOfLife SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (Stencil.cl ${CMAKE_CURRENT_BINARY_DIR}/Stencil.cl)
configure_file (MapOverlap.cl ${CMAKE_CURRENT_BINARY_DIR}/MapOverlap.cl)

# directory where data is written to
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)
