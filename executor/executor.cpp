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

KernelArg::~KernelArg()
{
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

void initSkelCL(int platformId, int deviceId)
{
  skelcl::init(skelcl::platform(platformId), skelcl::device(deviceId));
  pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Warning);
}

void initSkelCL(std::string deviceTypeString)
{
  skelcl::device_type deviceType;
  std::istringstream(deviceTypeString) >> deviceType;
  skelcl::init(skelcl::nDevices(1).deviceType(deviceType));
  pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Warning);
}

void shutdownSkelCL()
{
  skelcl::terminate();
}

std::string getPlatformName()
{
  auto& devicePtr = skelcl::detail::globalDeviceList.front();
  return devicePtr->clPlatform().getInfo<CL_PLATFORM_NAME>();
}

std::string getDeviceName()
{
  auto& devicePtr = skelcl::detail::globalDeviceList.front();
  return devicePtr->name();
}

std::string getDeviceType()
{
  auto& devicePtr = skelcl::detail::globalDeviceList.front();
  return devicePtr->typeAsString();
}

cl::Kernel buildKernel(const std::string& kernelCode,
                       const std::string& kernelName,
                       const std::string& buildOptions)
{
  auto& devPtr = skelcl::detail::globalDeviceList.front();

  auto p = skelcl::detail::Program(kernelCode,
                                   skelcl::detail::util::hash(kernelCode));
  p.build(buildOptions);

  return cl::Kernel(p.kernel(*devPtr, kernelName));
}

double executeKernel(cl::Kernel kernel,
               int localSize1, int localSize2, int localSize3,
               int globalSize1, int globalSize2, int globalSize3,
               const std::vector<KernelArg*>& args)
{
  auto& devPtr = skelcl::detail::globalDeviceList.front();

  cl_uint clLocalSize1 = localSize1;
  cl_uint clGlobalSize1 = globalSize1;
  cl_uint clLocalSize2 = localSize2;
  cl_uint clGlobalSize2 = globalSize2;
  cl_uint clLocalSize3 = localSize3;
  cl_uint clGlobalSize3 = globalSize3;

  int i = 0;
  for (auto& arg : args) {
    arg->upload();
    arg->setAsKernelArg(kernel, i);
    ++i;
  }

  auto event = devPtr->enqueue(kernel,
                               cl::NDRange(clGlobalSize1,
                                           clGlobalSize2, clGlobalSize3),
                               cl::NDRange(clLocalSize1,
                                           clLocalSize2, clLocalSize3));

  for (auto& arg : args) arg->download();

  return getRuntimeInMilliseconds(event);
}

double execute(const std::string& kernelCode, const std::string& kernelName,
               const std::string& buildOptions,
               int localSize1, int localSize2, int localSize3,
               int globalSize1, int globalSize2, int globalSize3,
               const std::vector<KernelArg*>& args)
{
  auto kernel = buildKernel(kernelCode, kernelName, buildOptions);
  return executeKernel(kernel, localSize1, localSize2, localSize3,
                       globalSize1, globalSize2, globalSize3, args);
}

