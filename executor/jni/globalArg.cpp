#include "opencl_executor_GlobalArg.h"
#include "handle.h"
#include "../executor.h"

jobject Java_opencl_executor_GlobalArg_createInput(JNIEnv* env, jclass cls,
                                                   jfloatArray data)
{
  auto arrayPtr = env->GetFloatArrayElements(data, nullptr);
  auto ptr = GlobalArg::create(arrayPtr,
                               env->GetArrayLength(data) * sizeof(float));
  env->ReleaseFloatArrayElements(data, arrayPtr, JNI_ABORT);

  auto methodID = env->GetMethodID(cls, "<init>", "(J)V"); 
  auto obj = env->NewObject(cls, methodID, ptr);
  return obj;
}

jobject Java_opencl_executor_GlobalArg_createOutput(JNIEnv* env, jclass cls,
                                                    jint size)
{
  auto ptr = GlobalArg::create(size);
  auto methodID = env->GetMethodID(cls, "<init>", "(J)V"); 
  auto obj = env->NewObject(cls, methodID, ptr);
  return obj;
}

jfloat Java_opencl_executor_GlobalArg_at(JNIEnv* env, jobject obj, jint index)
{
   auto ptr = getHandle<GlobalArg>(env, obj);
   auto vec = ptr->data();
   vec.copyDataToHost();
   auto dataPtr = reinterpret_cast<float*>(vec.hostBuffer().data());
   return dataPtr[index];
}

