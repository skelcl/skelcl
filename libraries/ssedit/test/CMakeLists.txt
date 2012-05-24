# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories ("${PROJECT_SOURCE_DIR}/libraries/gtest/include")
link_directories ("${CMAKE_BINARY_DIR}/libraries/gtest")

include_directories (${SSEDIT_COMMON_INCLUDE_DIR})
link_directories (${SSEDIT_COMMON_LIB_DIR})

# specify Test cases
add_executable (EntityTests EntityTests.cpp)
target_link_libraries (EntityTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (EntityTests ${CMAKE_CURRENT_BINARY_DIR}/EntityTests)

add_executable (CursorTests CursorTests.cpp)
target_link_libraries (CursorTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (CursorTests ${CMAKE_CURRENT_BINARY_DIR}/CursorTests)

add_executable (TypeTests TypeTests.cpp)
target_link_libraries (TypeTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (TypeTests ${CMAKE_CURRENT_BINARY_DIR}/TypeTests)

add_executable (RenameFunctionTests RenameFunctionTests.cpp)
target_link_libraries (RenameFunctionTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (RenameFunctionTests ${CMAKE_CURRENT_BINARY_DIR}/RenameFunctionTests)

add_executable (RenameTypedefTests RenameTypedefTests.cpp)
target_link_libraries (RenameTypedefTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (RenameTypedefTests ${CMAKE_CURRENT_BINARY_DIR}/RenameTypedefTests)

add_executable (OpenSourceFileTests OpenSourceFileTests.cpp)
target_link_libraries (OpenSourceFileTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (OpenSourceFileTests ${CMAKE_CURRENT_BINARY_DIR}/OpenSourceFileTests)

add_executable (FunctionInfoTests FunctionInfoTests.cpp)
target_link_libraries (FunctionInfoTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (FunctionInfoTests ${CMAKE_CURRENT_BINARY_DIR}/FunctionInfoTests)

add_executable (ParameterInfoTests ParameterInfoTests.cpp)
target_link_libraries (ParameterInfoTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (ParameterInfoTests ${CMAKE_CURRENT_BINARY_DIR}/ParameterInfoTests)

add_executable (TypedefInfoTests TypedefInfoTests.cpp)
target_link_libraries (TypedefInfoTests ssedit gtest_main ${SSEDIT_COMMON_LIBS})
add_test (TypedefInfoTests ${CMAKE_CURRENT_BINARY_DIR}/TypedefInfoTests)

