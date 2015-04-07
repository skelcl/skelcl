set (SKELCL_TOOLS_OPENCLINFO_SOURCES OpenCLInfo.cpp)

# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories (${SKELCL_COMMON_INCLUDE_DIR})
link_directories (${SKELCL_COMMON_LIB_DIR})

add_executable (OpenCLInfo ${SKELCL_TOOLS_OPENCLINFO_SOURCES})
target_link_libraries (OpenCLInfo ${SKELCL_COMMON_LIBS})
