add_executable (SimpleBig main.cpp)
target_link_libraries (SimpleBig SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (kernels.cl ${CMAKE_CURRENT_BINARY_DIR}/kernels.cl)
