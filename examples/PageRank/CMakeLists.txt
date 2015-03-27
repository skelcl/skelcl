set (SKELCL_EXAMPLES_PAGERANK_SOURCES main.cpp)

add_executable (PageRank ${SKELCL_EXAMPLES_PAGERANK_SOURCES})
target_link_libraries (PageRank SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl)

# directory where data is written to
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)
