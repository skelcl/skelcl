#include <string>
#include <vector>
#include <memory>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/detail/Program.h>
#include <SkelCL/detail/DeviceList.h>

std::vector<std::unique_ptr<skelcl::Vector<int>>> vectors;

bool startsWith(const std::string& haystack, const std::string& needle)
{
  if (haystack.length() >= needle.length()) {
    return (0 == haystack.compare(0, needle.length(), needle));
  } else {
    return false;
  }
}

size_t parseArg(const std::string& s)
{
  auto open = s.find('(');
  auto close = s.find(')', open);
  return std::stoi(s.substr(open+1, close-open-1));
}

class KernelArg {
public:
  enum Type : size_t {
    GLOBAL,
    LOCAL,
    INT
  };

  static KernelArg globalArg(size_t size ) { return KernelArg(GLOBAL, size); }
  static KernelArg localArg(size_t size) { return KernelArg(LOCAL, size); }
  static KernelArg integer(size_t value) { return KernelArg(INT, value); }

  KernelArg() = default;
  KernelArg(const KernelArg&) = default;
  KernelArg& operator=(const KernelArg&) = default;

  Type type;
  size_t size;

private:
  KernelArg(Type typeP, size_t sizeP)
    : type(typeP), size(sizeP)
  {}
};

std::istream& operator>>(std::istream& stream, KernelArg& arg)
{
  std::string s;
  stream >> s;

       if (startsWith(s, "GLOBAL")) arg = KernelArg::globalArg(parseArg(s));
  else if (startsWith(s, "LOCAL"))  arg = KernelArg::localArg(parseArg(s));
  else if (startsWith(s, "INT"))    arg = KernelArg::integer(parseArg(s));
  else throw std::invalid_argument(
    "Could not parse (" + s + ") as KernelArg.");

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const KernelArg& arg)
{
  if (arg.type == KernelArg::Type::GLOBAL) return stream << "GLOBAL";
  if (arg.type == KernelArg::Type::LOCAL)  return stream << "LOCAL";
  if (arg.type == KernelArg::Type::INT)    return stream << "INT";

  throw std::logic_error("This point should never be reached.");
}

cl::Kernel buildKernel(const std::string& kernelCode,
                       const std::string& kernelName)
{
  auto& devPtr = skelcl::detail::globalDeviceList.front();

  auto p = skelcl::detail::Program(kernelCode);
  p.build();

  return cl::Kernel(p.kernel(*devPtr, kernelName));
}

void prepareAndSetArg(cl::Kernel kernel, const KernelArg& arg)
{
  static int i = 0;
  auto& devPtr = skelcl::detail::globalDeviceList.front();

  if (arg.type == KernelArg::Type::GLOBAL) {
    vectors.emplace_back(new skelcl::Vector<int>(arg.size));
    kernel.setArg(i, vectors.back()->deviceBuffer(*devPtr).clBuffer());
  }

  if (arg.type == KernelArg::Type::LOCAL) {
    kernel.setArg(i, cl::__local(arg.size));
  }

  if (arg.type == KernelArg::Type::INT) {
    kernel.setArg(i, static_cast<cl_int>(arg.size));
  }

  ++i;
}

void execute(cl::Kernel kernel, int localSize, int globalSize,
             const KernelArg& arg0, const KernelArg& arg1,
             const KernelArg& arg2)
{
  auto& devPtr = skelcl::detail::globalDeviceList.front();

  cl_uint clLocalSize = localSize;
  cl_uint clGlobalSize = globalSize;

  prepareAndSetArg(kernel, arg0);
  prepareAndSetArg(kernel, arg1);
  prepareAndSetArg(kernel, arg2);

  auto event = devPtr->enqueue(kernel, cl::NDRange(clGlobalSize),
                               cl::NDRange(clLocalSize));

  event.wait();
}

int main(int argc, char** argv)
{
  using namespace pvsutil::cmdline;
  using namespace skelcl;

  pvsutil::CLArgParser cmd(Description("Executes a given OpenCL kernel."));

  auto deviceType = Arg<device_type>(Flags(Long("device_type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto enableLogging = Arg<bool>(Flags(Long("logging"),
                                       Long("verbose_logging")),
                                 Description("Enable verbose logging."),
                                 Default(false));

  auto kernelSource =
      Arg<std::string>(Flags(Long("kernelSource"), Short('s')),
                       Description("Source code of the OpenCL kernel"));

  auto kernelName = Arg<std::string>(Flags(Long("kernelName")),
                                     Description("Name of the kernel function"),
                                     Default(std::string("KERNEL")));

  auto localSize = Arg<int>(Flags(Long("localSize"), Short('l')),
                            Description("Local size used in execution"));

  auto globalSize = Arg<int>(Flags(Long("globalSize"), Short('g')),
                             Description("Global size used in execution"));

  auto kernelArg0 =
      Arg<KernelArg>(Flags(Long("arg0")), Description("KernelArg"));

  auto kernelArg1 =
      Arg<KernelArg>(Flags(Long("arg1")), Description("KernelArg"));

  auto kernelArg2 =
      Arg<KernelArg>(Flags(Long("arg2")), Description("KernelArg"));

  cmd.add(&deviceType, &enableLogging, &kernelSource, &kernelName, &localSize,
          &globalSize, &kernelArg0, &kernelArg1, &kernelArg2);
  cmd.parse(argc, argv);

  if (enableLogging) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  auto deviceCount = 1;
  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

  auto kernel = buildKernel(kernelSource, kernelName);
  execute(kernel, localSize, globalSize, kernelArg0, kernelArg1, kernelArg2);
}

