#include "executor.h"

namespace {
 
double getRuntimeInMilliseconds(cl::Event event)
{
  cl_ulong start;
  cl_ulong end;
  cl_int err;

  event.wait();

  err = clGetEventProfilingInfo(event(), CL_PROFILING_COMMAND_START,
                                sizeof(start), &start, NULL);
  ASSERT(err == CL_SUCCESS);

  err = clGetEventProfilingInfo(event(), CL_PROFILING_COMMAND_END,
                                sizeof(end), &end, NULL);
  ASSERT(err == CL_SUCCESS);

  return (end - start) * 1.0e-06;
}

}

GlobalArg::GlobalArg(skelcl::Vector<char>&& skelclVectorP, bool isOutputP)
  : skelclVector(std::move(skelclVectorP)), isOutput(isOutputP)
{
}

KernelArg* GlobalArg::create(void* data, size_t size, bool isOutput)
{
  auto dataCharPtr = static_cast<char*>(data);
  skelcl::Vector<char> skelclVector(dataCharPtr, dataCharPtr+size);
  return new GlobalArg{std::move(skelclVector), isOutput};
}

KernelArg* GlobalArg::create(size_t size, bool isOutput)
{
  skelcl::Vector<char> skelclVector;
  skelclVector.resize(size);
  return new GlobalArg{std::move(skelclVector), isOutput};
}

const skelcl::Vector<char>& GlobalArg::data() const
{
  return skelclVector;
}

void GlobalArg::setAsKernelArg(cl::Kernel kernel, int i)
{
  auto& devPtr = skelcl::detail::globalDeviceList.front();
  kernel.setArg(i, skelclVector.deviceBuffer(*devPtr).clBuffer());
}

void GlobalArg::upload()
{
  // set distribution
  skelcl::distribution::setSingle(skelclVector);
  // create buffers on device
  skelclVector.createDeviceBuffers();
  // start upload
  skelclVector.startUpload();
}

void GlobalArg::download()
{
  if (isOutput) {
    skelclVector.dataOnDeviceModified();
    skelclVector.copyDataToHost();
  }
}

LocalArg::LocalArg(size_t sizeP)
  : size(sizeP)
{
}

KernelArg* LocalArg::create(size_t size)
{
  return new LocalArg{size};
}

void LocalArg::setAsKernelArg(cl::Kernel kernel, int i)
{
  kernel.setArg(i, cl::__local(size)); 
}

void LocalArg::upload() {}
void LocalArg::download() {}

ValueArg::ValueArg(std::vector<char>&& valueP)
  : value(std::move(valueP))
{
}

KernelArg* ValueArg::create(void* data, size_t size)
{
  auto dataCharPtr = static_cast<char*>(data);
  std::vector<char> value(dataCharPtr, dataCharPtr+size);

  return new ValueArg{std::move(value)};
}

void ValueArg::setAsKernelArg(cl::Kernel kernel, int i)
{
  kernel.setArg(i, value.size(), value.data());
}

void ValueArg::upload() {}
void ValueArg::download() {}

void initSkelCL(std::string deviceTypeString)
{
  pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Warning);
  skelcl::device_type deviceType;
  std::istringstream(deviceTypeString) >> deviceType;
  skelcl::init(skelcl::nDevices(1).deviceType(deviceType));
}

void shutdownSkelCL()
{
  skelcl::terminate();
}

cl::Kernel buildKernel(const std::string& kernelCode,
                       const std::string& kernelName)
{
  auto& devPtr = skelcl::detail::globalDeviceList.front();

  auto p = skelcl::detail::Program(kernelCode);
  p.build();

  return cl::Kernel(p.kernel(*devPtr, kernelName));
}

double executeKernel(cl::Kernel kernel, int localSize, int globalSize,
                     const std::vector<KernelArg*>& args)
{
  auto& devPtr = skelcl::detail::globalDeviceList.front();

  cl_uint clLocalSize = localSize;
  cl_uint clGlobalSize = globalSize;

  int i = 0;
  for (auto& arg : args) {
    arg->upload();
    arg->setAsKernelArg(kernel, i);
    ++i;
  }

  auto event = devPtr->enqueue(kernel, cl::NDRange(clGlobalSize),
                                       cl::NDRange(clLocalSize));

  for (auto& arg : args) arg->download();

  return getRuntimeInMilliseconds(event);
}

double execute(std::string kernelCode, std::string kernelName, int localSize,
               int globalSize,
               const std::vector<KernelArg*>& args)
{
  auto kernel = buildKernel(kernelCode, kernelName);
  return executeKernel(kernel, localSize, globalSize, args);
}

