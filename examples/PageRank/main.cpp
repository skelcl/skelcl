///
/// \author Chris Cummins <chrisc.101@gmail.com>
///

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdlib>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Map.h>
#include <SkelCL/Vector.h>

using namespace skelcl;

template<typename T>
void printMatrix(Matrix<T> &m) {
    const size_t max = 5;
    const size_t rows = std::min(max, m.rowCount());
    const size_t cols = std::min(max, m.columnCount());

    for (size_t j = 0; j < rows; j++) {
        for (size_t i = 0; i < cols; i++)
            std::cout << m[j][i] << " ";
        std::cout << "\n";
    }
}

template<typename T>
void pageRank(const unsigned int iterations,
              const size_t numPages,
              const double dampingFactor,
              const double threshold)
{
    Vector<T> pageRanks({numPages});
    Matrix<int> pages({numPages, numPages});
    Vector<int> outLinks({numPages});
    Vector<T> deltas({numPages});
    Vector<T> nzeroes({numPages});

    T maxdelta = static_cast<T>(10000);

    const int sparsity = 3;

    // Initialise page ranks.
    for (auto i = pageRanks.begin(); i != pageRanks.end(); i++)
        *i = 1 / static_cast<T>(numPages);

    // Populate pages with links.
    for (size_t j = 0; j < pages.rowCount(); j++) {
        for (size_t i = 0; i < pages.columnCount(); i++) {
            if (i != j && !(rand() % sparsity)) {
                pages[j][i] = 1;
                outLinks[i] += 1;
            }
        }
    }

    printMatrix(pages);

    // Algorithm.
    unsigned int i = 0;
    T prev = static_cast<T>(1);
    auto delta = static_cast<T>(1.0);

    while (i < iterations && maxdelta > static_cast<T>(threshold)) {
        // Update maxdelta.
        maxdelta = 0;
        for (auto i = deltas.begin(); i != deltas.end(); i++)
            maxdelta = std::max(*i, maxdelta);

        // TODO:

        // Update counters.
        i++;
    }

    LOG_INFO("Exiting after ", i, " iterations.");
    LOG_INFO("Maximum delta ", maxdelta, ".");
}

int main(int argc, char** argv)
{
    using namespace pvsutil;
    using namespace pvsutil::cmdline;

    pvsutil::CLArgParser cmd(Description("The PageRank algorithm of Google Search."));

    // Parse arguments.
    auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                             Description("Enable verbose logging."),
                             Default(false));

    auto iterations = Arg<size_t>(Flags(Short('i'), Long("iterations")),
                                  Description("Number of iterations."),
                                  Default<size_t>(100));

    auto size = Arg<size_t>(Flags(Short('s'), Long("size")),
                            Description("Dataset size."),
                            Default<size_t>(1024));

    auto dampingFactor = Arg<double>(Flags(Short('d'), Long("damping-factor")),
                            Description("Damping factor."),
                            Default<double>(0.8));

    auto threshold = Arg<double>(Flags(Short('t'), Long("threshold")),
                                 Description("Threshold"),
                                 Default<double>(0.001));

    auto deviceCount = Arg<int>(Flags(Long("device-count")),
                                Description("Number of devices used by SkelCL."),
                                Default(1));

    auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                       Description("Device type: ANY, CPU, "
                                                   "GPU, ACCELERATOR"),
                                       Default(device_type::ANY));

    cmd.add(&verbose, &iterations, &size, &dampingFactor, &threshold,
            &deviceCount, &deviceType);
    cmd.parse(argc, argv);

    if (verbose)
        defaultLogger.setLoggingLevel(Logger::Severity::DebugInfo);

    init(nDevices(deviceCount).deviceType(deviceType));

    pageRank<double>(iterations, size, dampingFactor, threshold);

    return 0;
}
