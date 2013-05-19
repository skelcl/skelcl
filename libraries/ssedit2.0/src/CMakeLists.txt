set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSEDIT2.0_CXX_FLAGS}")

# set include dir
include_directories (
  "${PROJECT_SOURCE_DIR}/include"
  "${SSEDIT2.0_COMMON_INCLUDE_DIR}"
                    )

link_directories (${SSEDIT2.0_COMMON_LIB_DIR})

# set files used to build library
set (SSEDIT2.0_SOURCES
  CustomToolInvocation.cpp
  RefactoringTool.cpp
  RenameFunctionCallback.cpp
  RenameTypedefCallback.cpp
  TransferArgumentsCallback.cpp
  TransferParametersCallback.cpp
  Source.cpp
    )

# specify library target
add_library (ssedit2.0 ${SSEDIT2.0_SOURCES})
target_link_libraries (ssedit2.0 ${SSEDIT2.0_COMMON_LIBS})


