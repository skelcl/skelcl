/*
 * glaettenMapOverlap.cpp
 *
 *  Created on: 02.03.2011
 *      Author: Stefan Breuer
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

long long now()
{
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

void writePPM(Matrix<int>& img, const std::string filename)
{
  std::ofstream outputFile(filename.c_str());
  outputFile << "P2\n"
             << "#Creator: sbr\n" << img.columnCount() << " " << img.rowCount()
             << "\n255\n";

  for (auto itr = img.begin(); itr != img.end(); ++itr) {
    outputFile << *itr << "\n";
  }
}

int readPPM(const std::string inFile, std::vector<int>& img)
{
  img.clear();
  int numrows = 0, numcols = 0;
  std::ifstream infile(inFile);

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
  std::stringstream sstream(line);
  sstream >> numcols >> numrows;
   
  getNextLine(infile); // skip the next line (max value)

  if (version == "P2") {
    int i;
    while (getline(infile, line)) {
      std::stringstream ss(line);
      ss >> i;
      img.push_back(i);
    }
  } else {
    ASSERT_MESSAGE(false, "Wrong version of input graphics");
  }
  return numcols;
}

int main(int argc, char** argv)
{
  using namespace pvsutil::cmdline;

  pvsutil::CLArgParser cmd(Description("Computation of the Gaussian blur."));

  auto deviceCount =
      Arg<int>(Flags(Long("device_count")),
               Description("Number of devices used by SkelCL."), Default(1));

  auto deviceType = Arg<device_type>(Flags(Long("device_type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto range = Arg<int>(Flags(Long("range")), Description("The Overlap radius"),
                        Default(5));

  auto inFile = Arg<std::string>(Flags(Long("inFile")),
                                 Description("Filename of the input file"),
                                 Default(std::string("lena.pgm")));

  auto iterations = Arg<int>(Flags(Long("iterations"), Short('i')),
                             Description("Number of iterations."), Default(1));

  cmd.add(&deviceCount, &deviceType, &range, &inFile, &iterations);
  cmd.parse(argc, argv);

  // pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);

  std::stringstream outFile;
  outFile << inFile.getValue().substr(0, inFile.getValue().find(".")) << "_"
          << range << "_devs_" << deviceCount << ".pgm";


  skelcl::Vector<float> kernelVec(2 * range + 1);
  auto fwhm = 5;
  auto offset = (2 * range + 1) / 2;
  auto a = (fwhm / 2.354);
  for (auto i = -offset; i <= ((2 * range + 1) - offset - 1); i++) {
    kernelVec[i + offset] = exp(-i * i / (2 * a * a));
  }

  std::vector<int> img;
  int numcols = readPPM(inFile, img);

  auto time0 = now();

  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

  auto time1 = now();

  Matrix<int> inputImage(img, numcols);

  skelcl::MapOverlap<int(int)> s(std::ifstream{"./gauss2D.cl"},
                                 range.getValue(), detail::Padding::NEAREST,
                                 0);

  auto time2 = now();

  for (auto iter = 0; iter < iterations; iter++) {
    inputImage = s(inputImage, kernelVec, range.getValue());
    inputImage.copyDataToHost();
    inputImage.resize(inputImage.size());
  }

  auto time3 = now();

  inputImage.copyDataToHost();

  auto time5 = now();

  printf("Init time : %.12f\n", (float)(time1 - time0) / 1000000);
  printf("Creation time : %.12f\n", (float)(time2 - time1) / 1000000);
  printf("Exec time : %.12f\n", (float)(time3 - time2) / 1000000);
  printf("Total time : %.12f\n", (float)(time5 - time0) / 1000000);
  printf("Total without init time : %.12f\n", (float)(time5 - time1) / 1000000);

  writePPM(inputImage, outFile.str());

  skelcl::terminate();
}

