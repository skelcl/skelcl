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

long long get_time() {
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

void writePPM(Matrix<float>& img, const std::string filename) {
  std::ofstream outputFile(filename.c_str());

  outputFile << "P2\n" << "#Creator: sbr\n" << img.columnCount() << " "
	     << img.rowCount() << "\n255\n";

  Matrix<float>::iterator itr;

  for (itr = img.begin(); itr != img.end(); ++itr) {
    outputFile << (int) *itr << "\n";
  }
}

void writePPM(Matrix<int>& img, const std::string filename) {
  std::ofstream outputFile(filename.c_str());
  outputFile << "P2\n" << "#Creator: sbr\n" << img.columnCount() << " "
	     << img.rowCount() << "\n255\n";
  Matrix<int>::iterator itr = img.begin();
  for (itr = img.begin(); itr != img.end(); ++itr) {
    outputFile << *itr << "\n";
  }
}

void writePPM(std::vector<float>& img, const std::string filename) {
  std::ofstream outputFile(filename.c_str());

  outputFile << "P2\n" << "#Creator: sbr\n" << 640 << " " << 400 << "\n255\n";

  std::vector<float>::iterator itr;
  for (itr = img.begin(); itr != img.end(); ++itr) {
    outputFile << *itr << "\n";
  }
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

int main(int argc, char** argv) {
  long long time0;
  long long time1;
  long long time2;
  long long time3;
  long long time4;

  using namespace pvsutil::cmdline;
  pvsutil::CLArgParser cmd(Description("Computation of the Gaussian blur."));

  auto deviceCount = Arg<unsigned int>(Flags(Long("device_count")),
				       Description("Number of devices used by SkelCL."), Default(1u));

  auto deviceType = Arg<device_type>(Flags(Long("device_type")),
				     Description("Device type: ANY, CPU, "
						 "GPU, ACCELERATOR"), Default(device_type::ANY));

  auto rangeNorth = Arg<unsigned int>(Flags(Long("rangeNorth")),
				      Description("The range in north direction"), Default(5u));
  auto rangeSouth = Arg<unsigned int>(Flags(Long("rangeSouth")),
				      Description("The range in south direction"), Default(5u));
  auto rangeEast = Arg<unsigned int>(Flags(Long("rangeEast")),
				     Description("The range in east direction"), Default(5u));
  auto rangeWest = Arg<unsigned int>(Flags(Long("rangeWest")),
				     Description("The range in west direction"), Default(5u));
  auto iterationen = Arg<unsigned int>(Flags(Long("iterationen")),
				       Description("The number of iterations"), Default(1u));
  auto iterationenBetweenSwaps = Arg<int>(Flags(Long("iterationenSwap")),
					  Description("The number of iterations between Swaps"), Default(-1));
  auto inFile = Arg<std::string>(Flags(Long("inFile")),
				 Description("Filename of the input file"),
				 Default(std::string("data/lena.pgm")));

  cmd.add(&deviceCount, &deviceType, &rangeNorth, &rangeWest, &rangeSouth,
	  &rangeEast, &inFile, &iterationen, &iterationenBetweenSwaps);
  cmd.parse(argc, argv);

  std::stringstream out("_");

  out     << static_cast<std::string>(inFile).substr(0,
						     static_cast<std::string>(inFile).find(".")) << "_n_"
	  << rangeNorth << "_w_" << rangeWest << "_s_" << rangeSouth << "_e_"
	  << rangeEast << "_iter_" << iterationen << "_iterBS_"
	  << iterationenBetweenSwaps << "_devs_" << deviceCount << ".pgm";

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

  skelcl::Vector<float> kernelVec(2 * range + 1);

  /* set up kernel to weight the pixels */
  /* (KERNEL_SIZE - offset -1) is the CORRECT version */
  for (auto i = -offset; i <= ((2 * static_cast<int>(range) + 1) - offset - 1); i++) {
    kernelVec[i + offset] = exp(-i * i / (2 * a * a));
  }

  //Read pgm-File
  std::vector<float> img(1);

  int numcols = readPPM(inFile, img);

  time0 = get_time();

  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

  time1 = get_time();

  Matrix<float> inputImage(img, numcols);

  time2 = get_time();

  skelcl::Stencil<float(float)> s(std::ifstream { "./gauss2D.cl" },
				  static_cast<unsigned int>(rangeNorth),
				  static_cast<unsigned int>(rangeWest),
				  static_cast<unsigned int>(rangeSouth),
				  static_cast<unsigned int>(rangeEast),
				  detail::Padding::NEAREST, 255,
				  "func", static_cast<unsigned int>(iterationenBetweenSwaps));
  time3 = get_time();
  Matrix<float> outputImage = s(iterationen, inputImage, kernelVec,
				static_cast<int>(range));
  time4 = get_time();

  printf("Init time : %.12f\n", (float) (time1 - time0) / 1000000);
  printf("Input time : %.12f\n", (float) (time2 - time1) / 1000000);
  printf("Creation time : %.12f\n", (float) (time3 - time2) / 1000000);
  printf("Exec time : %.12f\n", (float) (time4 - time3) / 1000000);
  printf("Total time : %.12f\n", (float) (time4 - time0) / 1000000);
  printf("Total without init time : %.12f\n",
	 (float) (time4 - time1) / 1000000);

  writePPM(outputImage, out.str());

  skelcl::terminate();

}
