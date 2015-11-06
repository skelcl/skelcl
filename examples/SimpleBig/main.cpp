#include "common.h"

typedef float DATA_T;

#define NORTH 1
#define WEST 10
#define SOUTH 30
#define EAST 30


SKELCL_ADD_DEFINE(NORTH)
SKELCL_ADD_DEFINE(WEST)
SKELCL_ADD_DEFINE(SOUTH)
SKELCL_ADD_DEFINE(EAST)

int main(int argc, char** argv)
{
        using namespace pvsutil;
        using namespace pvsutil::cmdline;

        CLArgParser cmd(Description("Garbage kernel."));

        // Parse arguments.
        auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                                 Description("Enable verbose logging."),
                                 Default(false));

        auto iterations = Arg<size_t>(Flags(Short('i'), Long("iterations")),
                                      Description("Number of iterations."),
                                      Default<size_t>(10));

        auto deviceCount = Arg<int>(Flags(Long("device-count")),
                                    Description("Number of devices used by SkelCL."),
                                    Default(1));

        auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                           Description("Device type: ANY, CPU, "
                                                       "GPU, ACCELERATOR"),
                                           Default(device_type::ANY));

        // auto swaps = Arg<int>(Flags(Short('S'), Long("iterations-between-swaps")),
        //                       Description("The number of iterations "
        //                                   "between swaps"),
        //                       Default(-1));

        auto width = Arg<size_t>(Flags(Short('w'), Long("width")),
                                 Description("Dataset width."),
                                 Default<size_t>(1024));

        auto height = Arg<size_t>(Flags(Short('h'), Long("height")),
                                  Description("Dataset height."),
                                  Default<size_t>(1024));

        auto complex = Arg<bool>(Flags(Short('c'), Long("complex")),
                                 Description("Use complex kernel."),
                                 Default(false));

        cmd.add(&verbose, &iterations, &deviceCount, &deviceType, &width, &height, &complex);
        cmd.parse(argc, argv);

        if (verbose)
                defaultLogger.setLoggingLevel(Logger::Severity::DebugInfo);

        init(nDevices(deviceCount).deviceType(deviceType));


        // Create input and populate with random values.
        Matrix<DATA_T> input({height, width});
        for (size_t y = 0; y < input.rowCount(); y++)
                for (size_t x = 0; x < input.columnCount(); x++)
                        input[y][x] = static_cast<DATA_T>(rand());

        // Create stencil.
        Stencil<DATA_T(DATA_T)> s(std::ifstream{"./kernels.cl"}, complex ? "complex" : "simple",
                                  stencilShape(north(NORTH),west(WEST),south(SOUTH),east(EAST)),
                                  Padding::NEUTRAL, 0);
                                  // , swaps);

        // Run stencil.
        Matrix<DATA_T> output = s.toSeq()(iterations, input);

        // Copy data back to host.
        output.copyDataToHost();

        return 0;
}
