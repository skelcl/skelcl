# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories ("${PROJECT_SOURCE_DIR}/libraries/gtest/include")
link_directories ("${CMAKE_BINARY_DIR}/libraries/gtest")

macro (add_testcase _name)
  add_executable (${_name} ${_name}.cpp)
  target_link_libraries (${_name} pvsutil gtest_main)
  add_test (${_name} ${CMAKE_CURRENT_BINARY_DIR}/${_name})
endmacro()

# specify Test cases
#add_testcase (AssertTests)
#add_testcase (LoggerTests)
add_testcase (TimerTests)

