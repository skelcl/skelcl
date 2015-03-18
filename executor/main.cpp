#include <string>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include "executor.h"

int main(int argc, char** argv)
{
  using namespace pvsutil::cmdline;
  using namespace skelcl;

  pvsutil::CLArgParser cmd(Description("Executes a given OpenCL kernel."));

  auto deviceType = Arg<std::string>(Flags(Long("device_type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(std::string("ANY")));

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

  cmd.add(&deviceType, &enableLogging, &kernelSource, &kernelName, &localSize,
          &globalSize);
  cmd.parse(argc, argv);

  if (enableLogging) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }


  // example usage
  initSkelCL();

  std::vector<char> vc(1024);
  std::fill(vc.begin(), vc.end(), 5);

  std::vector<char> result(1024);

  std::vector<KernelArg*> args;
  args.emplace_back(GlobalArg::create(vc.data(), vc.size()));
  args.emplace_back(GlobalArg::create(result.data(), result.size(), true));

  execute(kernelSource, kernelName, localSize, 1, 1, globalSize, 1, 1, args);

  LOG_INFO("done");
  auto& res = dynamic_cast<GlobalArg*>(args.back())->data();
  auto s = 0;
  for (auto& i : res) {
    s += i;
  }
  LOG_INFO("res: ", s);

  for(auto& a : args) delete(a);

  shutdownSkelCL();
}

