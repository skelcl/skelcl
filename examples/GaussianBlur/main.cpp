/*****************************************************************************
 * Copyright (c) 2011-2012 The skelcl Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of skelcl.                                              *
 * skelcl is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * skelcl is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * skelcl is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * For non-commercial academic use see the license specified in the file     *
 * "LICENSE-academic.txt".                                                   *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * If you are interested in other licensing models, including a commercial-  *
 * license, please contact the author at michel.steuwer@uni-muenster.de      *
 *                                                                           *
 *****************************************************************************/

///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
/// \author Stefan Breuer <s_breu03@uni-muenster.de>
///
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
#include <SkelCL/StencilSequence.h>
#include <SkelCL/Padding.h>

#include <chrono>

using namespace skelcl;

// Border handling behaviour.
static const Padding paddingType = Padding::NEUTRAL;

long long getTime()
{
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

template<typename T>
void writePPM(T& img, const std::string filename) {
  std::ofstream outputFile(filename.c_str());
  typename T::iterator itr;

  outputFile << "P2\n"
             << "#Creator: skelcl\n" << img.columnCount() << " "
             << img.rowCount() << "\n255\n";

  for (itr = img.begin(); itr != img.end(); ++itr)
    outputFile << static_cast<int>(*itr) << "\n";
}

int readPPM(const std::string inFile, std::vector<float>& img) {
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
  // Third line : size
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

int main(int argc, char** argv) {

  using namespace pvsutil::cmdline;
  pvsutil::CLArgParser cmd(Description("Computation of Gaussian blur."));

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

  auto oneD = Arg<bool>(Flags(Short('y'), Long("one-d")),
                        Description("One dimensional."),
                        Default(false));

  auto deviceCount = Arg<int>(Flags(Long("device-count")),
                              Description("Number of devices used by SkelCL."),
                              Default(1));

  auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto rangeNorth = Arg<int>(Flags(Long("range-north")),
                             Description("The range in north direction"),
                             Default(5));

  auto rangeSouth = Arg<int>(Flags(Long("range-south")),
                             Description("The range in south direction"),
                             Default(5));

  auto rangeEast = Arg<int>(Flags(Long("range-east")),
                            Description("The range in east direction"),
                            Default(5));

  auto rangeWest = Arg<int>(Flags(Long("range-west")),
                            Description("The range in west direction"),
                            Default(5));

  auto iterations = Arg<int>(Flags(Short('i'), Long("iterations")),
                             Description("The number of iterations"),
                             Default(1));

  auto swaps = Arg<int>(Flags(Short('S'), Long("iterations-between-swaps")),
                        Description("The number of iterations "
                                    "between swaps"),
                        Default(-1));

  cmd.add(&verbose, &inFile, &outFile, &useMapOverlap, &oneD, &deviceCount,
          &deviceType, &rangeNorth, &rangeSouth, &rangeEast, &rangeWest,
          &iterations, &swaps);
  cmd.parse(argc, argv);

  if (verbose) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  auto range = rangeNorth > rangeSouth ? rangeNorth : rangeSouth;
  range = range > rangeEast ? range : rangeEast;
  range = range > rangeWest ? range : rangeWest;

  //calculate the kernel
  int fwhm = 5;
  int offset = (2 * range + 1) / 2;

  /*
   * Given as parameter
   * FWHM = 2 sqrt(2 ln2) sigma ~ 2.35 sigma
   */
  float a = (fwhm / 2.354);

  Vector<float> kernelVec(2 * range + 1);

  /* set up kernel to weight the pixels */
  /* (KERNEL_SIZE - offset -1) is the CORRECT version */
  for (auto i = -offset; i <= ((2 * range + 1) - offset - 1); i++)
    kernelVec[i + offset] = exp(-i * i / (2 * a * a));

  std::vector<float> img(1);
  int numcols = readPPM(inFile, img);

  Matrix<float> image(img, numcols);

  init(nDevices(deviceCount).deviceType(deviceType));

  auto start = getTime();

  if (useMapOverlap) {
    if (oneD) {
      MapOverlap<float(float)>s(std::ifstream { "./MapOverlap1D.cl" },
                                range.getValue(), paddingType, 0);

      for (auto i = 0; i < iterations; i++)
        image = s(image, kernelVec, range.getValue());
    } else {
      MapOverlap<float(float)>s(std::ifstream{"./MapOverlap2D.cl"},
                                range.getValue(), paddingType, 0);

      for (auto i = 0; i < iterations; i++)
        image = s(image, kernelVec, range.getValue());
    }
  } else {
    if (oneD) {
      Stencil<float(float)>s(std::ifstream { "./Stencil1D.cl" }, "func",
                             stencilShape(any(0),
                                          north(range),
                                          south(range)),
                             Padding::NEUTRAL, 255);

      image = s(image, kernelVec, range.getValue());
    } else {
      Stencil<float(float)> blur(std::ifstream { "./Stencil2D.cl" }, "func",
                                 stencilShape(north(rangeNorth),
                                              south(rangeSouth),
                                              west(rangeWest),
                                              east(rangeEast)),
                                 Padding::NEUTRAL, 255);

      if (iterations > 1) {
        StencilSequence<float(float)> sequence;
        sequence.add(&blur);

        image = sequence(iterations, image, kernelVec, range.getValue());
      } else {
        image = blur(image, kernelVec, range.getValue());
      }
    }
  }

  auto end = getTime();

  writePPM(image, outFile);

  terminate();

  printf("Image size:   %lu x %lu px\n", image.columnCount(), image.rowCount());
  printf("Elapsed time: %lld ms\n", end - start);
  printf("Output:       %s\n", outFile.getValue().c_str());
}
