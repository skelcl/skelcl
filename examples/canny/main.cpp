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

long long get_time()
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
                                 Default(std::string("data/lena.pgm")));

  cmd.add(&deviceCount, &deviceType, &range, &inFile);
  cmd.parse(argc, argv);

  std::stringstream outFile("_");
  outFile << inFile.getValue().substr(0, inFile.getValue().find("."))
          << "_" << range << "_devs_" << deviceCount << ".pgm";

  auto fwhm = 5;
  auto offset = (2 * range + 1) / 2;
  auto a = (fwhm / 2.354);

  skelcl::Vector<float> kernelVec(2 * range + 1);

  for (auto i = -offset; i <= ((2 * range + 1) - offset - 1); i++) {
    kernelVec[i + offset] = exp(-i * i / (2 * a * a));
  }

  std::vector<float> img(1); // why 1?
  auto numcols = readPPM(inFile, img);

  auto time0 = get_time();

  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

  Matrix<float> inputImage(img, numcols);

  auto time1 = get_time();

  skelcl::MapOverlap<float(float)> m(std::ifstream{"./cannyGauss.cl"},
                                     range.getValue(), detail::Padding::NEUTRAL,
                                     255, "func");
  skelcl::MapOverlap<float(float)> n(std::ifstream{"./cannySobel.cl"}, 1,
                                     detail::Padding::NEAREST, 0, "func");
  skelcl::MapOverlap<float(float)> o(std::ifstream{"./cannyNMS.cl"}, 1,
                                     detail::Padding::NEUTRAL, 0, "func");
  skelcl::MapOverlap<float(float)> p(std::ifstream{"./cannyThreshold.cl"}, 1,
                                     detail::Padding::NEAREST, 255, "func");

  auto time2 = get_time();

  Matrix<float> outputImage = m(inputImage, kernelVec, range.getValue());
  outputImage.copyDataToHost();
  outputImage.resize(inputImage.size());

  auto time3 = get_time();

  Matrix<float> tempImage = n(outputImage, kernelVec, 1);
  tempImage.copyDataToHost();
  tempImage.resize(inputImage.size());

  auto time4 = get_time();

  outputImage = o(tempImage, kernelVec, 1);
  outputImage.copyDataToHost();
  outputImage.resize(inputImage.size());

  auto time5 = get_time();

  tempImage = p(outputImage, kernelVec, 1);

  auto time6 = get_time();

  tempImage.copyDataToHost();

  auto time7 = get_time();

  printf("Total Init time: %.12f\n", (float)(time1 - time0) / 1000000);
  printf("Total Creation time: %.12f\n", (float)(time2 - time1) / 1000000);
  printf("Total Gauß time: %.12f\n", (float)(time3 - time2) / 1000000);
  printf("Total Sobel time: %.12f\n", (float)(time4 - time3) / 1000000);
  printf("Total NSM time: %.12f\n", (float)(time5 - time4) / 1000000);
  printf("Total Threshold time: %.12f\n", (float)(time6 - time5) / 1000000);
  printf("Total Total time: %.12f\n", (float)(time7 - time0) / 1000000);
  printf("Total Total no init time: %.12f\n", (float)(time7 - time1) / 1000000);

  writePPM(tempImage, outFile.str());

  skelcl::terminate();
}

