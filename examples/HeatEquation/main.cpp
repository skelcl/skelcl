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
#include <SkelCL/detail/Padding.h>

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
                             Default(10));

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

  auto swaps = Arg<int>(Flags(Short('S'), Long("iterations-between-swaps")),
                        Description("The number of iterations "
                                    "between swaps"),
                        Default(-1));

  auto size = Arg<size_t>(Flags(Short('s'), Long("size")),
                          Description("Size of the grid"),
                          Default<size_t>(6144));


  cmd.add(&verbose, &iterations, &useMapOverlap, &deviceType, &deviceCount,
          &swaps, &size);
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
  for (size_t i = 0; i < size * size; i++)
    img.push_back(0);

  // Create a white square in the center of the data.
  int i, j;
  for (i = size / 2; i < size / 2 + sqrt(size); i++)
    for (j = size / 2; j < size / 2 + sqrt(size); j++)
      img[i * size + j] = 255;

  init(nDevices(deviceCount).deviceType(deviceType));

  Matrix<float> image(img, size);

  auto start = getTime();

  if (useMapOverlap) {
    MapOverlap<float(float)> s(std::ifstream { "./MapOverlap.cl" }, 1,
                               detail::Padding::NEUTRAL, 0, "func");

    for (int i = 0; i < iterations; i++) {
      image = s(image);
      image.copyDataToHost();
      image.resize(image.size());
    }
  } else {
    Stencil<float(float)> s(std::ifstream { "./Stencil.cl" }, 1, 1, 1, 1,
                            detail::Padding::NEUTRAL, 0, "func", swaps);

    image = s(iterations, image);
  }

  Matrix<float>::iterator itr = image.begin();
  auto end = getTime();

  writePPM(image, out.str());

  terminate();

  printf("Iterations:   %d\n", iterations.getValue());
  printf("Image size:   %lu x %lu px\n", image.rowCount(), image.columnCount());
  printf("Elapsed time: %lld ms\n", end - start);
  printf("Output:       %s\n", out.str().c_str());
}
