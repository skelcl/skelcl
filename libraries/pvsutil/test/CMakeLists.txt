# set include dir for all examples
include_directories ("${PVSUTIL_COMMON_INCLUDE_DIR}")
include_directories ("${GTEST_INCLUDE_DIR}")

macro (add_testcase _name)
  add_executable (${_name} ${_name}.cpp)
  target_link_libraries (${_name} pvsutil gtest_main)
  add_test (${_name} ${CMAKE_CURRENT_BINARY_DIR}/${_name})
endmacro()

# specify Test cases
#add_testcase (AssertTests)
#add_testcase (LoggerTests)
add_testcase (TimerTests)

