#include <string>
#include <vector>

#include "opencl_executor_Executor.h"
#include "handle.h"
#include "../executor.h"

void
  Java_opencl_executor_Executor_execute(JNIEnv* env, jclass cls,
                                        jstring jKernelSource,
                                        jstring jKernelName, jint localSize,
                                        jint globalSize, jobjectArray jArgs)
{
  (void)cls;

  auto kernelSourcePtr = env->GetStringUTFChars(jKernelSource, nullptr);
  std::string kernelSource{kernelSourcePtr};
  auto kernelNamePtr = env->GetStringUTFChars(jKernelName, nullptr);
  std::string kernelName{kernelNamePtr};

  std::vector<KernelArg*> args(env->GetArrayLength(jArgs));
  int i = 0;
  for (auto& p : args) {
    auto obj = env->GetObjectArrayElement(jArgs, i);
    p = getHandle<KernelArg>(env, obj);
    ++i;
  }

  execute(kernelSource, kernelName, localSize, globalSize, args);

  env->ReleaseStringUTFChars(jKernelSource, kernelSourcePtr);
  env->ReleaseStringUTFChars(jKernelName, kernelNamePtr);
}

