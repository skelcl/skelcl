/*
 * Author: Stefan Breuer
 */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/IndexMatrix.h>
#include <SkelCL/MapOverlap.h>
#include <SkelCL/Stencil.h>
#include <SkelCL/Padding.h>

#include <chrono>

using namespace skelcl;

long long getTime()
{
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

// Write the contents of "grid" to "filename".
void writeGrid(Matrix<int> &grid, const std::string filename) {
  std::ofstream outputFile(filename.c_str());
  size_t i, j;

  for (i = 0; i < grid.rowCount(); i++) {
    for (j = 0; j < grid.columnCount(); j++)
      outputFile << grid[i][j];
    outputFile << "\n";
  }
}


int main(int argc, char** argv)
{
  using namespace pvsutil::cmdline;

  pvsutil::CLArgParser cmd(Description("Conway's Game of Life."));

  // Parse arguments.
  auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                           Description("Enable verbose logging."),
                           Default(false));

  auto useMapOverlap = Arg<bool>(Flags(Short('m'), Long("map-overlap")),
                                 Description("Use the MapOverlap skeleton rather "
                                             "than Stencil."),
                                 Default(false));

  auto deviceCount = Arg<int>(Flags(Long("device-count")),
                              Description("Number of devices used by SkelCL."),
                              Default(1));

  auto size = Arg<int>(Flags(Short('s'), Long("size")),
                       Description("Grid size [s,s]."),
                       Default(4096));

  auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto iterations = Arg<int>(Flags(Short('i'), Long("iterations")),
                             Description("The number of iterations"),
                             Default(10));

  auto swaps = Arg<int>(Flags(Short('S'), Long("iterations-between-swaps")),
                        Description("The number of iterations "
                                    "between swaps"),
                        Default(-1));

  auto outFile = Arg<std::string>(Flags(Short('o'), Long("output")),
                                  Description("Path to output data file"),
                                  Default(std::string("data/grid.txt")));

  cmd.add(&verbose, &useMapOverlap, &deviceCount, &deviceType,
          &iterations, &swaps, &size, &outFile);
  cmd.parse(argc, argv);

  if (verbose) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  const int numcols = size;
  const int numrows = size;
  std::vector<int> img(0, 0);

  for (auto i = 0; i < numcols * numrows; i++)
    img.push_back(random() > (INT_MAX / 2));

  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

  Matrix<int> grid(img, numcols);

  long long start;

  if (useMapOverlap) {
    // MapOverlap.
    skelcl::MapOverlap<int(int)> s(std::ifstream{"./MapOverlap.cl"}, 1,
                                   Padding::NEUTRAL, 0, "func");

    start = getTime();
    for (auto i = 0; i < iterations; i++)
      grid = s(grid);

    grid.resize(grid.size());
  } else {
    // Stencil.
    skelcl::Stencil<int(int)> s(std::ifstream{"./Stencil.cl"}, "func",
                                stencilShape(any(1)),
                                Padding::NEUTRAL, 0);
    start = getTime();
    grid = s(iterations, grid);
  }

  grid.copyDataToHost();
  auto end = getTime();

  writeGrid(grid, outFile);

  skelcl::terminate();

  printf("Iterations:   %d\n", iterations.getValue());
  printf("Grid size:    %lu x %lu\n", grid.columnCount(), grid.rowCount());
  printf("Elapsed time: %lld ms\n", end - start);
  printf("Output:       %s\n", outFile.getValue().c_str());
}
