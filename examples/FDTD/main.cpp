#include "Simulation.h"

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include "SkelCL/SkelCL.h"
#include "SkelCL/detail/DeviceProperties.h"

#include <fstream>

long long getTime()
{
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

int main(int argc, char *argv[])
{
  using namespace pvsutil::cmdline;

  pvsutil::CLArgParser cmd(Description("Finite-Difference Time-Domain."));

  // Parse arguments.
  auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                           Description("Enable verbose logging."),
                           Default(false));

  auto size = Arg<int>(Flags(Short('s'), Long("size")),
                       Description("Simulation size: [s, s]."),
                       Default(2048));

  auto resolution = Arg<int>(Flags(Short('r'), Long("resolution")),
                             Description("Simulation resolution."),
                             Default(100));

  auto deviceCount = Arg<int>(Flags(Long("device-count")),
                              Description("Number of devices used by SkelCL."),
                              Default(1));

  auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto swaps = Arg<int>(Flags(Short('S'), Long("iterations-between-swaps")),
                        Description("The number of iterations "
                                    "between swaps"),
                        Default(-1));

  cmd.add(&verbose, &size, &resolution, &deviceCount, &deviceType, &swaps);
  cmd.parse(argc, argv);

  if (verbose) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

  auto start = getTime();

  Simulation sim(size, resolution, swaps);
  sim.initialize();
  sim.run();

  auto end = getTime();

  skelcl::terminate();

  printf("Simulation size: %d x %d\n", static_cast<int>(size), static_cast<int>(size));
  printf("Resolution:      %d\n", static_cast<int>(resolution));
  printf("Elapsed time:    %lld ms\n", end - start);
}
