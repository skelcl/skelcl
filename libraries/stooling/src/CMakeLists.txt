set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STOOLING_CXX_FLAGS}")

# set include dir
include_directories (${STOOLING_COMMON_INCLUDE_DIR})

link_directories (${STOOLING_COMMON_LIB_DIR})

# set files used to build library
set (STOOLING_SOURCES
  ../include/stooling/SourceCode.h
  ../include/stooling/detail/stoolingDll.h
  CustomToolInvocation.cpp
  FixKernelParameterCallback.cpp
  GetParameterTypeNamesCallback.cpp
  RefactoringTool.cpp
  RenameFunctionCallback.cpp
  RenameTypedefCallback.cpp
  RedefineTypedefCallback.cpp
  TransferArgumentsCallback.cpp
  TransferParametersCallback.cpp
  SourceCode.cpp
    )

# specify library target
add_library (stooling ${STOOLING_SOURCES})
target_link_libraries (stooling ${STOOLING_COMMON_LIBS})

# define DLL export Macro
if (WIN32)
  set_target_properties(stooling PROPERTIES DEFINE_SYMBOL STOOLING_EXPORTS)
endif (WIN32)


