set (SKELCL_EXAMPLES_CANNY_SOURCES
      main.cpp
    )

add_executable (canny ${SKELCL_EXAMPLES_CANNY_SOURCES})
target_link_libraries (canny SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (cannyGauss.cl ${CMAKE_CURRENT_BINARY_DIR}/cannyGauss.cl)
configure_file (cannyNMS.cl ${CMAKE_CURRENT_BINARY_DIR}/cannyNMS.cl)
configure_file (cannySobel.cl ${CMAKE_CURRENT_BINARY_DIR}/cannySobel.cl)
configure_file (cannyThreshold.cl ${CMAKE_CURRENT_BINARY_DIR}/cannyThreshold.cl)
configure_file (data/lena.pgm ${CMAKE_CURRENT_BINARY_DIR}/data/lena.pgm)
