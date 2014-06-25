#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include <string>
#include <vector>
#include <memory>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/detail/Program.h>
#include <SkelCL/detail/DeviceList.h>

class KernelArg {
public:
  virtual void setAsKernelArg(cl::Kernel kernel, int i) = 0;
  virtual void upload() = 0;
  virtual void download() = 0;
};

class GlobalArg : public KernelArg {
public:
  static std::unique_ptr<KernelArg> create(void* data, size_t sizeInBytes,
                                           bool isOutput = false);
  static std::unique_ptr<KernelArg> create(size_t sizeInBytes,
                                           bool isOutput = false);

  void setAsKernelArg(cl::Kernel kernel, int i);
  void upload();
  void download();

  const skelcl::Vector<char>& data() const;
  
private:
  GlobalArg(skelcl::Vector<char>&& skelclVectorP, bool isOutputP);

  skelcl::Vector<char> skelclVector;
  bool isOutput;
};

class LocalArg : public KernelArg {
public:
  static std::unique_ptr<KernelArg> create(size_t sizeInBytes);

  void setAsKernelArg(cl::Kernel kernel, int i);
  void upload();
  void download();

private:
  LocalArg(size_t sizeP);

  size_t size;
};

class ValueArg : public KernelArg {
public:
  static std::unique_ptr<KernelArg> create(void* data, size_t sizeInBytes);

  void setAsKernelArg(cl::Kernel kernel, int i);
  void upload();
  void download();

private:
  ValueArg(std::vector<char>&& valueP);

  std::vector<char> value;
};

std::istream& operator>>(std::istream& stream, KernelArg& arg);

std::ostream& operator<<(std::ostream& stream, const KernelArg& arg);

void initSkelCL(std::string deviceType = std::string("ANY"));

void shutdownSkelCL();

cl::Kernel buildKernel(const std::string& kernelCode,
                       const std::string& kernelName);

void executeKernel(cl::Kernel kernel, int localSize, int globalSize,
                   const std::vector<std::unique_ptr<KernelArg>>& args);

void execute(std::string kernelCode, std::string kernelName, int localSize,
             int globalSize,
             const std::vector<std::unique_ptr<KernelArg>>& args);

#endif // EXECUTOR_H_
