set (SKELCL_EXAMPLES_GAMEOFLIFE_SOURCES
      main.cpp
    )

add_executable (gameoflife ${SKELCL_EXAMPLES_GAMEOFLIFE_SOURCES})
target_link_libraries (gameoflife SkelCL ${SKELCL_COMMON_LIBS})

# copy file along with binary
configure_file (gameoflife.cl ${CMAKE_CURRENT_BINARY_DIR}/gameoflife.cl)



