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

#include <pvsutil/Logger.h>
#include <pvsutil/CLArgParser.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/Zip.h>
#include <SkelCL/Reduce.h>

using namespace skelcl;

long long getTime() {
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

template<typename ForwardIterator>
void init(ForwardIterator begin, ForwardIterator end) {
  srand((unsigned) time(0));
  while (begin != end) {
    *begin = (int) (((float) rand() / (float) RAND_MAX) * 125);
    ++begin;
  }
}

int main(int argc, char** argv) {
  using namespace pvsutil::cmdline;

  pvsutil::CLArgParser cmd(Description("Computation of the dot product of two "
                                       "randomly created vectors."));

  // Parse arguments.
  auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                           Description("Enable verbose logging."),
                           Default(false));

  auto deviceCount = Arg<int>(Flags(Long("device-count")),
                              Description("Number of devices used by SkelCL."),
                              Default(1));

  auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto size = Arg<int>(Flags(Short('s'), Long("size")),
		       Description("Size of the two vectors used in "
				   "the computation."),
                       Default(16777216));

  auto skipCheck = Arg<bool>(Flags(Long("skip-check")),
                             Description("Don't check the computed result "
                                         "against a sequential computed "
                                         "version."),
                             Default(false));

  cmd.add(&verbose, &deviceType, &deviceCount, &size, &skipCheck);
  cmd.parse(argc, argv);

  if (verbose) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  init(nDevices(deviceCount).deviceType(deviceType));

  Zip<int(int, int)> mult("int func(int x, int y){ return x*y; }");
  Reduce<int(int)>    sum("int func(int x, int y){ return x+y; }", "0");

  Vector<int> A(size);
  Vector<int> B(size);

  init(A.begin(), A.end());
  init(B.begin(), B.end());

  auto start = getTime();
  int C = sum(mult(A, B)).front();
  auto end = getTime();

  // Compare expected results.
  if (!skipCheck) {
    int res = 0;

    for (size_t i = 0; i < A.size(); ++i)
      res += A[i] * B[i];

    if (res != C) {
      LOG_ERROR("Expected: ", res,
                ", received: ", C,
                ", difference: ", res - C);
      ABORT_WITH_ERROR("Expected result does not match computed result.");
    }
  }

  terminate();

  printf("Vector size:  %lu\n", A.size());
  printf("Solution:     %d\n", C);
  printf("Elapsed time: %lld ms\n", end - start);

  return 0;
}
