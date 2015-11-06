///
/// \author Stefan Breuer <s_breu03@uni-muenster.de>
///
#include <algorithm>
#include <chrono>
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

using namespace skelcl;

long long getTime() {
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

template<typename T>
void writePPM(T& img, const std::string filename) {
  std::ofstream outputFile(filename.c_str());
  typename T::iterator itr;

  outputFile << "P2\n"
             << "#Creator: sbr\n"
             << img.columnCount() << " "
	     << img.rowCount()
             << "\n255\n";

  for (itr = img.begin(); itr != img.end(); ++itr)
    outputFile << static_cast<int>(*itr) << "\n";
}

int main(int argc, char** argv) {
  using namespace pvsutil::cmdline;

  pvsutil::CLArgParser cmd(Description("Computation of the Heat equation."));

  // Parse arguments.
  auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                           Description("Enable verbose logging."),
                           Default(false));

  auto iterations = Arg<int>(Flags(Short('i'), Long("iterations")),
                             Description("The number of iterations"),
                             Default(100));

  auto useMapOverlap = Arg<bool>(Flags(Short('m'), Long("map-overlap")),
                                 Description("Use the MapOverlap skeleton rather "
                                             "than Stencil."),
                                 Default(false));

  auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto deviceCount = Arg<int>(Flags(Long("device-count")),
                              Description("Number of devices used by SkelCL."),
                              Default(1));

  // auto swaps = Arg<int>(Flags(Short('S'), Long("iterations-between-swaps")),
  //                       Description("The number of iterations "
  //                                   "between swaps"),
  //                       Default(-1));

  auto size = Arg<size_t>(Flags(Short('s'), Long("size")),
                          Description("Size of the grid"),
                          Default<size_t>(4096));

  cmd.add(&verbose, &iterations, &useMapOverlap, &deviceType, &deviceCount, &size);
  cmd.parse(argc, argv);

  if (verbose) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  // Get output file path.
  std::stringstream out("_");
  out << "data/" << size << "_dev" << deviceCount << "_iter" << iterations << ".pgm";

  // Input data.
  std::vector<float> img(1);
  img.clear();

  // Create input dataset.
  size_t i, j;
  size_t vborder = size / 15;
  size_t hborder = size / 6;
  for (j = 0; j < size; j++)
    for (i = 0; i < size; i++) {
      float v = 50;

      if (j < vborder || j > size - vborder)
        v = 255;

      if (i < hborder)
        v = 150;

      if (i > size - hborder)
        v = 0;

      img.push_back(v);
    }

  init(nDevices(deviceCount).deviceType(deviceType));

  Matrix<float> image(img, size);
  long long start;

  if (useMapOverlap) {
    MapOverlap<float(float)> s(std::ifstream { "./MapOverlap.cl" }, 1,
                               Padding::NEUTRAL, 0, "func");

    start = getTime();
    for (int i = 0; i < iterations; i++)
      image = s(image);

    image.resize(image.size());
  } else {
    Stencil<float(float)> s(std::ifstream { "./Stencil.cl" }, "func",
                            stencilShape(any(1)),
                            Padding::NEUTRAL, 0);

    start = getTime();
    image = s.toSeq()(iterations, image);
  }
  image.copyDataToHost();

  auto end = getTime();

  Matrix<float>::iterator itr = image.begin();

  writePPM(image, out.str());

  terminate();

  printf("Iterations:   %d\n", iterations.getValue());
  printf("Image size:   %lu x %lu px\n", image.rowCount(), image.columnCount());
  printf("Elapsed time: %lld ms\n", end - start);
  printf("Output:       %s\n", out.str().c_str());
}
