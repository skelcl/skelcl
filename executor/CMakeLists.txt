find_package(JNI)

# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include" ${JNI_INCLUDE_DIRS})

include_directories (${SKELCL_COMMON_INCLUDE_DIR})
link_directories (${SKELCL_COMMON_LIB_DIR})

add_library (libexecutor executor.cpp
                         jni/handle.cpp
                         jni/executor.cpp 
                         jni/globalArg.cpp 
                         jni/kernelArg.cpp 
                         jni/localArg.cpp
                         jni/valueArg.cpp)

target_link_libraries (libexecutor SkelCL ${SKELCL_COMMON_LIBS})

add_executable (executor main.cpp)
target_link_libraries (executor libexecutor)

