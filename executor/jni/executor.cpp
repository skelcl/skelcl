#include <string>
#include <vector>
#include <limits>

#include "opencl_executor_Executor.h"
#include "handle.h"
#include "../executor.h"

jdouble
  Java_opencl_executor_Executor_execute(JNIEnv* env, jclass,
                                        jstring jKernelSource,
                                        jstring jKernelName,
                                        jstring jBuildOptions,
                                        jint localSize1, jint localSize2, jint localSize3,
                                        jint globalSize1, jint globalSize2, jint globalSize3,
                                        jobjectArray jArgs)
{
  auto kernelSourcePtr = env->GetStringUTFChars(jKernelSource, nullptr);
  std::string kernelSource{kernelSourcePtr};
  auto kernelNamePtr = env->GetStringUTFChars(jKernelName, nullptr);
  std::string kernelName{kernelNamePtr};
  auto buildOptionsPtr = env->GetStringUTFChars(jBuildOptions, nullptr);
  std::string buildOptions{buildOptionsPtr};

  std::vector<KernelArg*> args(env->GetArrayLength(jArgs));
  int i = 0;
  for (auto& p : args) {
    auto obj = env->GetObjectArrayElement(jArgs, i);
    p = getHandle<KernelArg>(env, obj);
    ++i;
  }

  auto runtime = execute(kernelSource, kernelName, buildOptions,
   localSize1, localSize2, localSize3, globalSize1, globalSize2, globalSize3, args);

  env->ReleaseStringUTFChars(jKernelSource, kernelSourcePtr);
  env->ReleaseStringUTFChars(jKernelName, kernelNamePtr);
  return runtime;
}

void Java_opencl_executor_Executor_init(JNIEnv *, jclass,
                                        jint platformId,
                                        jint deviceId)
{
  initSkelCL(platformId, deviceId);
}

jstring Java_opencl_executor_Executor_getPlatformName(JNIEnv * env, jclass)
{
  auto name = getPlatformName();
  return env->NewStringUTF(name.c_str());
}

jlong Java_opencl_executor_Executor_getDeviceLocalMemSize(JNIEnv *, jclass)
{
  const auto value = getDeviceLocalMemSize();
  ASSERT_MESSAGE(value <= std::numeric_limits<long>::max(), "JNI cast overflow");
  return static_cast<long>(value);
}

jlong Java_opencl_executor_Executor_getDeviceGlobalMemSize(JNIEnv *, jclass)
{
  const auto value = getDeviceGlobalMemSize();
  ASSERT_MESSAGE(value <= std::numeric_limits<long>::max(), "JNI cast overflow");
  return static_cast<long>(value);
}

jlong Java_opencl_executor_Executor_getDeviceMaxMemAllocSize(JNIEnv *, jclass)
{
  const auto value = getDeviceMaxMemAllocSize();
  ASSERT_MESSAGE(value <= std::numeric_limits<long>::max(), "JNI cast overflow");
  return static_cast<long>(value);
}

jstring Java_opencl_executor_Executor_getDeviceName(JNIEnv * env, jclass)
{
  auto name = getDeviceName();
  return env->NewStringUTF(name.c_str());
}

jstring Java_opencl_executor_Executor_getDeviceType(JNIEnv * env, jclass)
{
  auto type = getDeviceType();
  return env->NewStringUTF(type.c_str());
}

void Java_opencl_executor_Executor_init__(JNIEnv *, jclass)
{
  initSkelCL("ANY");
}

void Java_opencl_executor_Executor_shutdown(JNIEnv *, jclass)
{
  shutdownSkelCL();
}

