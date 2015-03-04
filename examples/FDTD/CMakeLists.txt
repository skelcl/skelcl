set (SKELCL_EXAMPLES_FDTD_SOURCES main.cpp Simulation.cpp)

add_executable (FDTD ${SKELCL_EXAMPLES_FDTD_SOURCES})
target_link_libraries (FDTD SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (Stencils.cl ${CMAKE_CURRENT_BINARY_DIR}/Stencils.cl)
configure_file (Kernels.cl ${CMAKE_CURRENT_BINARY_DIR}/Kernels.cl)
configure_file (data/job.rmf ${CMAKE_CURRENT_BINARY_DIR}/data/job.rmf)
