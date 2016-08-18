/*****************************************************************************
 * Copyright (c) 2011-2012 The SkelCL Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of SkelCL.                                              *
 * SkelCL is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * SkelCL is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * SkelCL is distributed in the hope that it will be useful,                 *
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
///

#include <cstdlib>
#include <ctime>

#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>
#include <pvsutil/Timer.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/IndexVector.h>
#include <SkelCL/Map.h>

using namespace skelcl;

const double epsilon = 1e-03;

template <typename ForwardIterator>
void fillVector(ForwardIterator begin, ForwardIterator end)
{
  srand( (unsigned)time(0) );
  while (begin != end) {
    *begin = ( (float)rand()/(float)RAND_MAX ) * 125.0f;
    ++begin;
  }
}

float fillScalar()
{
  srand( (unsigned)time(0) );
  return ((float)rand()/(float)RAND_MAX) * 125.0f;
}

std::vector<float> montecarloCPU(int size) {
  // pre allocate output
  std::vector<float> output(size);

  for (auto idx = 0; idx < size; ++idx) {
    int iter = 25000;

    float sum = 0.0;
    long seed = idx;

    for (int i = 0; i < iter; i++) {
      seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
      seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);

      // this generates a number between 0 and 1 (with an awful entropy)
      float x = ((float) (seed & 0x0FFFFFFF)) / 268435455;

      // repeat for y
      seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
      seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
      float y = ((float) (seed & 0x0FFFFFFF)) / 268435455;

      float dist = sqrt(x * x + y * y);
      if (dist <= 1.0f) { sum += 1.0f; }
    }

    sum = sum * 4;
    output[idx] = sum/iter;
  }

  return output;
}

void montecarlo(int size, bool checkResult)
{

  Map<float(Index)> mc(R"(
    float func(Index idx) {
      int iter = 25000;

      float sum = 0.0;
      long seed = idx;

      for (int i = 0; i < iter; i++) {
        seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
        seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);

        // this generates a number between 0 and 1 (with an awful entropy)
        float x = ((float) (seed & 0x0FFFFFFF)) / 268435455;

        // repeat for y
        seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
        seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
        float y = ((float) (seed & 0x0FFFFFFF)) / 268435455;

        float dist = sqrt(x * x + y * y);
        if (dist <= 1.0f) { sum += 1.0f; }
      }

      sum = sum * 4;
      return sum/iter;
    }
  )");

  Vector<Index> input(size);

  pvsutil::Timer timer;
  auto output = mc(input);
  output.copyDataToHost();
  pvsutil::Timer::time_type time = timer.stop();

  if (checkResult) {
    auto cpuOutput = montecarloCPU(size);
    if (output.size() != cpuOutput.size()) {
      LOG_ERROR("Output vectors have differnt sizes: ", output.size(), " and ", cpuOutput.size());
    } else {
      int errors = 0;
      for (int i = 0; i < size; ++i) {
        if ( fabs(output[i] - cpuOutput[i]) > epsilon ) { ++errors; }
      }
      if (errors > 0) {
        LOG_ERROR(errors, " errors detected.");
      } else {
        LOG_INFO("Performed check and no errors detected.");
      }
    }
  }

  LOG_INFO("Time: ", time, " ms");
}

int main(int argc, char** argv)
{
  using namespace pvsutil::cmdline;
  pvsutil::CLArgParser cmd(Description("MonteCarlo example."));

  auto deviceCount = Arg<int>(Flags(Long("device_count")),
                              Description("Number of devices used by SkelCL."),
                              Default(2));

  auto deviceType = Arg<device_type>(Flags(Long("device_type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto enableLogging = Arg<bool>(Flags(Short('l'), Long("logging"),
                                       Long("verbose_logging")),
                                 Description("Enable verbose logging."),
                                 Default(false));

  auto size = Arg<int>(Flags(Short('n'), Long("size")),
                       Description("Size of the two vectors used in "
                                   "the computation."),
                       Default(1024 * 1024));

  auto checkResult = Arg<bool>(Flags(Short('c'),
                                     Long("check"), Long("check_result")),
                               Description("Check SkelCL computation against "
                                           "sequential computed version."),
                               Default(false));

  cmd.add(deviceCount, deviceType, enableLogging, size, checkResult)
     .parse(argc, argv);

  if (enableLogging) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));
  montecarlo(size, checkResult);
  return 0;
}
