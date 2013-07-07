set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKELCL_CXX_FLAGS}")

# set include dir for all examples
include_directories (${STOOLING_COMMON_INCLUDE_DIR})
include_directories (${GTEST_INCLUDE_DIR})

link_directories (${STOOLING_COMMON_LIB_DIR})
link_directories (${CMAKE_CURRENT_BINARY_DIR})

macro (add_testcase _name)
  add_executable (${_name} ${_name}.cpp)
  target_link_libraries (${_name} stooling gtest_main ${STOOLING_COMMON_LIBS})
  add_test (${_name} ${CMAKE_CURRENT_BINARY_DIR}/${_name})
endmacro()

# specify Test cases
add_testcase (RenameFunctionTest)
add_testcase (RenameTypedefTest)
add_testcase (TransferParametersTest)
add_testcase (TransferArgumentsTest)

