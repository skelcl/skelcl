# set include dir
include_directories (
  "${PROJECT_SOURCE_DIR}/include"
   ${SSEDIT_COMMON_INCLUDE_DIR}
                    )

link_directories (${SSEDIT_COMMON_LIB_DIR})

add_executable (printAST printAST.cpp)

# specify library target
target_link_libraries (printAST ${SSEDIT_COMMON_LIBS})

