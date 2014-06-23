# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories (${SKELCL_COMMON_INCLUDE_DIR})
link_directories (${SKELCL_COMMON_LIB_DIR})

add_executable (executor main.cpp)
target_link_libraries (executor SkelCL ${SKELCL_COMMON_LIBS})

