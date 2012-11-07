# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories ("${PROJECT_SOURCE_DIR}/libraries/gtest/include")
link_directories ("${CMAKE_BINARY_DIR}/libraries/gtest")

include_directories (${SKELCL_COMMON_INCLUDE_DIR})
link_directories (${SKELCL_COMMON_LIB_DIR})

macro (add_testcase _name)
  add_executable (${_name} ${_name}.cpp)
  target_link_libraries (${_name} SkelCL gtest_main ${SKELCL_COMMON_LIBS})
  add_test (${_name} ${CMAKE_CURRENT_BINARY_DIR}/${_name})
endmacro()

# specify Test cases
add_testcase (DistributionTests)
add_testcase (DeviceTests)
add_testcase (DevicesTests)
add_testcase (MapTests)
add_testcase (ZipTests)
add_testcase (ReduceTests)
add_testcase (ProgramTests)
add_testcase (VectorTests)
add_testcase (SHA1Tests)
add_testcase (DeviceSelectionTests)
add_testcase (MatrixTests)
add_testcase (IndexVectorTests)
add_testcase (IndexMatrixTests)

