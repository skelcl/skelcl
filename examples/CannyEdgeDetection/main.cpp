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
#include <SkelCL/detail/Padding.h>

#include <chrono>

using namespace skelcl;

long long getTime()
{
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

void writePPM(Matrix<float>& img, const std::string filename)
{
  std::ofstream outputFile(filename.c_str());

  outputFile << "P2\n"
             << "#Creator: sbr\n" << img.columnCount() << " " << img.rowCount()
             << "\n255\n";

  Matrix<float>::iterator itr;

  for (itr = img.begin(); itr != img.end(); ++itr) {
    outputFile << (int)*itr << "\n";
  }
}

void writePPM(Matrix<int>& img, const std::string filename)
{
  std::ofstream outputFile(filename.c_str());
  outputFile << "P2\n"
             << "#Creator: sbr\n" << img.columnCount() << " " << img.rowCount()
             << "\n255\n";
  Matrix<int>::iterator itr = img.begin();
  for (itr = img.begin(); itr != img.end(); ++itr) {
    outputFile << *itr << "\n";
  }
}

void writePPM(std::vector<float>& img, const std::string filename)
{
  std::ofstream outputFile(filename.c_str());

  outputFile << "P2\n" << "#Creator: sbr\n" << 640 << " " << 400 << "\n255\n";

  std::vector<float>::iterator itr;
  for (itr = img.begin(); itr != img.end(); ++itr) {
    outputFile << *itr << "\n";
  }
}

int readPPM(const std::string inFile, std::vector<float>& img)
{
  img.clear();
  int numrows = 0, numcols = 0;
  std::ifstream infile(inFile);

  std::stringstream ss;
  std::string inputLine = "";

  // Check if file exists.
  if (!infile.good()) {
    LOG_ERROR("Input file '", inFile, "' not found!");
    abort();
  }

  // First line : version
  getline(infile, inputLine);

  // Second line : comment
  getline(infile, inputLine);

  // Continue with a stringstream
  getline(infile, inputLine);
  std::stringstream ss2(inputLine);
  //	// Third line : size
  ss2 >> numcols >> numrows;

  getline(infile, inputLine);

  int i;
  while (getline(infile, inputLine)) {
    std::stringstream ss(inputLine);
    ss >> i;
    img.push_back(i);
  }

  infile.close();
  return numcols;
}

int main(int argc, char** argv)
{
  using namespace pvsutil::cmdline;

  pvsutil::CLArgParser cmd(Description("Computation of Canny Edge Detection."));

  // Parse arguments.
  auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                           Description("Enable verbose logging."),
                           Default(false));

  auto inFile = Arg<std::string>(Flags(Short('i'), Long("input")),
                                 Description("Path of the input PGM file"),
                                 Default(std::string("data/lena.pgm")));

  auto outFile = Arg<std::string>(Flags(Short('o'), Long("output")),
                                  Description("Path of the output PGM file"),
                                  Default(std::string("data/lena.out.pgm")));

  auto useMapOverlap = Arg<bool>(Flags(Short('m'), Long("map-overlap")),
                              Description("Use the MapOverlap skeleton rather "
                                          "than Stencil."),
                              Default(false));

  auto deviceCount = Arg<int>(Flags(Long("device-count")),
                              Description("Number of devices used by SkelCL."),
                              Default(1));

  auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto range = Arg<int>(Flags(Short('r'), Long("radius")),
                        Description("Gaussian overlap radius"),
                        Default(5));

  cmd.add(&verbose, &useMapOverlap, &deviceCount, &deviceType,
          &range, &inFile, &outFile);
  cmd.parse(argc, argv);

  if (verbose) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  auto fwhm = 5;
  auto offset = (2 * range + 1) / 2;
  auto a = (fwhm / 2.354);

  skelcl::Vector<float> kernelVec(2 * range + 1);

  for (auto i = -offset; i <= ((2 * range + 1) - offset - 1); i++) {
    kernelVec[i + offset] = exp(-i * i / (2 * a * a));
  }

  std::vector<float> img(1); // why 1?
  auto numcols = readPPM(inFile, img);

  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

  Matrix<float> inputImage(img, numcols);
  Matrix<float> outputImage;

  auto start = getTime();

  if (useMapOverlap) {
    // Map Overlap.
    skelcl::MapOverlap<float(float)> m(std::ifstream{"./MapOverlapGauss.cl"},
                                       range.getValue(), detail::Padding::NEUTRAL,
                                       255, "func");
    skelcl::MapOverlap<float(float)> n(std::ifstream{"./MapOverlapSobel.cl"}, 1,
                                       detail::Padding::NEAREST, 0, "func");
    skelcl::MapOverlap<float(float)> o(std::ifstream{"./MapOverlapNMS.cl"}, 1,
                                       detail::Padding::NEUTRAL, 0, "func");
    skelcl::MapOverlap<float(float)> p(std::ifstream{"./MapOverlapThreshold.cl"}, 1,
                                       detail::Padding::NEAREST, 255, "func");

    outputImage = m(inputImage, kernelVec, range.getValue());
    outputImage.copyDataToHost();
    outputImage.resize(inputImage.size());

    Matrix<float> tempImage = n(outputImage, kernelVec, 1);
    tempImage.copyDataToHost();
    tempImage.resize(inputImage.size());

    outputImage = o(tempImage, kernelVec, 1);
    outputImage.copyDataToHost();
    outputImage.resize(inputImage.size());

    tempImage = p(outputImage, kernelVec, 1);
    tempImage.copyDataToHost();
  } else {
    // Stencil.
    skelcl::Stencil<float(float)> s(std::ifstream { "./StencilGauss.cl" },
                                    static_cast<int>(range), static_cast<int>(range),
                                    static_cast<int>(range), static_cast<int>(range),
                                    detail::Padding::NEUTRAL, 255, "func");
    s.add(std::ifstream { "./StencilSobel.cl" }, 1, 1, 1, 1,
          detail::Padding::NEAREST, 255, "func");
    s.add(std::ifstream { "./StencilNMS.cl" }, 1, 1, 1, 1,
          detail::Padding::NEUTRAL, 1, "func");
    s.add(std::ifstream { "./StencilThreshold.cl" }, 0, 0, 0, 0,
          detail::Padding::NEAREST, 1, "func");

    outputImage = s(1, inputImage, kernelVec, static_cast<int>(range));
  }

  auto end = getTime();

  writePPM(outputImage, outFile);

  skelcl::terminate();

  printf("Image size:   %lu x %lu px\n", inputImage.columnCount(), inputImage.rowCount());
  printf("Elapsed time: %lld ms\n", end - start);
  printf("Output:       %s\n", outFile.getValue().c_str());
}
