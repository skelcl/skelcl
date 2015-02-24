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
/// \author Stefan Breuer<s_breu03@uni-muenster.de>
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
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

using namespace skelcl;

template <typename T>
void writePPM(Matrix<T>& img, const std::string& filename)
{
  std::ofstream outputFile(filename);
  outputFile << "P2\n"
             << "#Creator: skelcl\n" << img.columnCount() << " "
             << img.rowCount() << "\n255\n";

  for (auto& pixel : img) {
    outputFile << pixel << "\n";
  }
}

template <typename T>
Matrix<T> readPPM(const std::string& inFile)
{
  auto numrows = 0u, numcols = 0u;
  std::ifstream infile(inFile);

  // Check if file exists.
  if (!infile.good()) {
    LOG_ERROR("Input file '", inFile, "' not found!");
    abort();
  }

  // function to get next line which is not a comment
  auto getNextLine = [](std::ifstream & infile)->std::string
  {
    std::string inputLine;
    while(true) {
      getline(infile, inputLine);
      // read until a line is not a comment
      if (inputLine[0] != '#') break;
    }
    return inputLine;
  };

  auto version = getNextLine(infile);

  auto line = getNextLine(infile);
  std::stringstream(line) >> numcols >> numrows;
   
  getNextLine(infile); // skip the next line (max value)

  Matrix<T> inputImage({numrows, numcols});
  auto iter = inputImage.begin();

  if (version == "P2") {
    while (getline(infile, line)) {
      std::stringstream(line) >> *iter;
      ++iter;
    }
  } else {
    ASSERT_MESSAGE(false, "Wrong version of input graphics");
  }

  return inputImage;
}


int main(int argc, char** argv)
{
  using namespace pvsutil::cmdline;

  // specify command line flags
  pvsutil::CLArgParser cmd(Description("Computation of the Gaussian blur."));

  auto deviceCount =
      Arg<int>(Flags(Long("device_count")),
               Description("Number of devices used by SkelCL."), Default(1));

  auto deviceType = Arg<device_type>(Flags(Long("device_type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto range = Arg<int>(Flags(Long("range"), Short('r')),
                        Description("The Overlap radius"), Default(5));

  auto inFile = Arg<std::string>(Flags(Long("inFile")),
                                 Description("Filename of the input file"),
                                 Default(std::string("data/lena.pgm")));

  cmd.add(&deviceCount, &deviceType, &range, &inFile);
  cmd.parse(argc, argv);

  // initialize skelcl
  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

  // build output file name: <inputFilename>_<range>_devs_<deviceCount>.pgm
  std::stringstream outFile;
  outFile << inFile.getValue().substr(0, inFile.getValue().find(".")) << "_"
          << range << "_devs_" << deviceCount << ".pgm";

  // vector of weights for the gaussian blur
  skelcl::Vector<float> weigths(2 * range + 1);
  auto fwhm = 5;
  auto offset = (2 * range + 1) / 2;
  auto a = (fwhm / 2.354);
  for (auto i = -offset; i <= ((2 * range + 1) - offset - 1); i++) {
    weigths[i + offset] = exp(-i * i / (2 * a * a));
  }

  // read image in a skelcl::Matrix of ints
  auto inputImage = readPPM<int>(inFile);

  skelcl::MapOverlap<int(int)> s(std::ifstream{"./gauss2D.cl"},
                                 range.getValue(), detail::Padding::NEAREST, 0);

  inputImage = s(inputImage, weigths, range.getValue());

  writePPM(inputImage, outFile.str());
}

