set (SKELCL_EXAMPLES_GAMEOFLIFESTENCIL_SOURCES
      main.cpp
    )

add_executable (gameoflifeStencil ${SKELCL_EXAMPLES_GAMEOFLIFESTENCIL_SOURCES})
target_link_libraries (gameoflifeStencil SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (gameoflife.cl ${CMAKE_CURRENT_BINARY_DIR}/gameoflife.cl)



