set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSE_CXX_FLAGS}")

# set include dir
include_directories (
  "${PROJECT_SOURCE_DIR}/include"
   ${SSEDIT_COMMON_INCLUDE_DIR}
                    )

link_directories (${SSEDIT_COMMON_LIB_DIR})

# set files used to build library
set (SSEDIT_SOURCES
      CallExpression.cpp
      Cursor.cpp
      Delta.cpp
      DeltaTree.cpp
      Entity.cpp
      Function.cpp
      Parameter.cpp
      SourceFile.cpp
      SourceLocation.cpp
      SourceRange.cpp
      TempSourceFile.cpp
      Type.cpp
      Typedef.cpp
    )

# specify library target
add_library (ssedit ${SSEDIT_SOURCES})
target_link_libraries (ssedit ${SSEDIT_COMMON_LIBS})

# specify install
if (OS_LINUX)
	# specify install into directory DESTINATION/lib
  install (TARGETS ssedit
					 LIBRARY DESTINATION lib
					 ARCHIVE DESTINATION lib
					)
endif (OS_LINUX)

