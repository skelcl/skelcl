set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKELCL_CXX_FLAGS}")

# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories ("${PROJECT_SOURCE_DIR}/libraries/gtest/include")
link_directories ("${CMAKE_BINARY_DIR}/libraries/gtest")

include_directories (${SSEDIT2.0_COMMON_INCLUDE_DIR})
link_directories (${SSEDIT2.0_COMMON_LIB_DIR})

macro (add_testcase _name)
  add_executable (${_name} ${_name}.cpp)
  target_link_libraries (${_name} ssedit2.0 gtest_main ${SSEDIT2.0_COMMON_LIBS})
  add_test (${_name} ${CMAKE_CURRENT_BINARY_DIR}/${_name})
endmacro()

# specify Test cases
add_testcase (RenameFunctionTest)
add_testcase (RenameTypedefTest)
add_testcase (TransferParametersTest)
add_testcase (TransferArgumentsTest)

