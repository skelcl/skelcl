#include "executor.h"

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

void executeKernel(cl::Kernel kernel, int localSize, int globalSize,
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

  devPtr->enqueue(kernel, cl::NDRange(clGlobalSize), cl::NDRange(clLocalSize));

  for (auto& arg : args) arg->download();
}

void execute(std::string kernelCode, std::string kernelName, int localSize,
             int globalSize,
             const std::vector<KernelArg*>& args)
{
  auto kernel = buildKernel(kernelCode, kernelName);
  executeKernel(kernel, localSize, globalSize, args);
}

