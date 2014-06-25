#include "opencl_executor_LocalArg.h"
#include "handle.h"
#include "../executor.h"

template <typename T>
jobject helper(JNIEnv* env, jclass cls, T value)
{
  auto ptr = ValueArg::create(&value, sizeof(value));
  auto methodID = env->GetMethodID(cls, "<init>", "(J)V"); 
  auto obj = env->NewObject(cls, methodID, ptr);
  return obj;
}

jobject Java_opencl_executor_ValueArg_create__F(JNIEnv* env, jclass cls,
                                                jfloat value)
{
  return helper(env, cls, value);
}

jobject Java_opencl_executor_ValueArg_create__I(JNIEnv* env, jclass cls,
                                                jint value)
{
  return helper(env, cls, value);
}

