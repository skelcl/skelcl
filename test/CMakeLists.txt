# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories ("${PROJECT_SOURCE_DIR}/libraries/gtest/include")
link_directories ("${CMAKE_BINARY_DIR}/libraries/gtest")

include_directories (${SKELCL_COMMON_INCLUDE_DIR})
link_directories (${SKELCL_COMMON_LIB_DIR})

# specify Test cases
add_executable (DistributionTests DistributionTests.cpp)
target_link_libraries (DistributionTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (DistributionTests ${CMAKE_CURRENT_BINARY_DIR}/DistributionTests)

add_executable (DeviceTests DeviceTests.cpp)
target_link_libraries (DeviceTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (DeviceTests ${CMAKE_CURRENT_BINARY_DIR}/DeviceTests)

add_executable (DevicesTests DevicesTests.cpp)
target_link_libraries (DevicesTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (DevicesTests ${CMAKE_CURRENT_BINARY_DIR}/DevicesTests)

add_executable (MapTests MapTests.cpp)
target_link_libraries (MapTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (MapTests ${CMAKE_CURRENT_BINARY_DIR}/MapTests)

add_executable (ZipTests ZipTests.cpp)
target_link_libraries (ZipTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (ZipTests ${CMAKE_CURRENT_BINARY_DIR}/ZipTests)

add_executable (ReduceTests ReduceTests.cpp)
target_link_libraries (ReduceTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (ReduceTests ${CMAKE_CURRENT_BINARY_DIR}/ReduceTests)

add_executable (ProgramTests ProgramTests.cpp)
target_link_libraries (ProgramTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (ProgramTests ${CMAKE_CURRENT_BINARY_DIR}/ProgramTests)

add_executable (VectorTests VectorTests.cpp)
target_link_libraries (VectorTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (VectorTests ${CMAKE_CURRENT_BINARY_DIR}/VectorTests)

add_executable (SHA1Tests SHA1Tests.cpp)
target_link_libraries (SHA1Tests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (SHA1Tests ${CMAKE_CURRENT_BINARY_DIR}/SHA1Tests)

add_executable (DeviceSelectionTests DeviceSelectionTests.cpp)
target_link_libraries (DeviceSelectionTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (DeviceSelectionTests ${CMAKE_CURRENT_BINARY_DIR}/DeviceSelectionTests)

add_executable (MatrixTests MatrixTests.cpp)
target_link_libraries (MatrixTests SkelCL gtest_main ${SKELCL_COMMON_LIBS})
add_test (MatrixTests ${CMAKE_CURRENT_BINARY_DIR}/MatrixTests)

