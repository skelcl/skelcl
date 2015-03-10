set (SKELCL_EXAMPLES_DOTPRODUCT_SOURCES main.cpp)

add_executable (DotProduct ${SKELCL_EXAMPLES_DOTPRODUCT_SOURCES})
target_link_libraries (DotProduct SkelCL ${SKELCL_COMMON_LIBS})
