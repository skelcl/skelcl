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

  auto size =
      Arg<int>(Flags(Long("problem_size"), Short('n')),
               Description("Number of devices used by SkelCL."), Default(5));

  auto iterationen =
      Arg<int>(Flags(Long("iterations"), Short('i')),
               Description("The number of iterations"), Default(1));

  auto iterationenBetweenSwaps = Arg<int>(
      Flags(Long("iterations_between_swap")),
      Description("The number of iterations between Swaps"), Default(-1));

  cmd.add(&deviceCount, &deviceType, &iterationen, &iterationenBetweenSwaps,
          &size);
  cmd.parse(argc, argv);

	int numcols = size;
  int numrows = size;

  std::vector<int> img(1, 1);

  for (auto i = 0; i < numcols * numrows; i++) {
    img.push_back(random() > (INT_MAX / 2));
  }

	auto time0 = get_time();

	skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

	auto time1 = get_time();

  Matrix<int> inputImage(img, numcols);

  auto time2 = get_time();

  skelcl::Stencil<int(int)> s(std::ifstream{"./gameoflife.cl"}, 1, 1, 1, 1,
                              detail::Padding::NEUTRAL, 0, "func",
                              static_cast<int>(iterationenBetweenSwaps));
  auto time3 = get_time();
  Matrix<int> outputImage = s(iterationen, inputImage);
  auto time4 = get_time();

  printf("Init time : %.12f\n", (float)(time1 - time0) / 1000000);
  printf("Input time : %.12f\n", (float)(time2 - time1) / 1000000);
  printf("Creation time : %.12f\n", (float)(time3 - time2) / 1000000);
  printf("Exec time : %.12f\n", (float)(time4 - time3) / 1000000);
  printf("Total time : %.12f\n", (float)(time4 - time0) / 1000000);
  printf("Total without init time : %.12f\n", (float)(time4 - time1) / 1000000);

  skelcl::terminate();
}

