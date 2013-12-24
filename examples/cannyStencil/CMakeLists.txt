set (SKELCL_EXAMPLES_CANNYSTENCIL_SOURCES
      main.cpp
    )

add_executable (cannyStencil ${SKELCL_EXAMPLES_CANNYSTENCIL_SOURCES})
target_link_libraries (cannyStencil SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (cannyGauss.cl ${CMAKE_CURRENT_BINARY_DIR}/cannyGauss.cl)
configure_file (cannyNMS.cl ${CMAKE_CURRENT_BINARY_DIR}/cannyNMS.cl)
configure_file (cannySobel.cl ${CMAKE_CURRENT_BINARY_DIR}/cannySobel.cl)
configure_file (cannyThreshold.cl ${CMAKE_CURRENT_BINARY_DIR}/cannyThreshold.cl)



