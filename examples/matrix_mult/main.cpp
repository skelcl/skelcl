/*****************************************************************************
 * Copyright (c) 2011-2013 The SkelCL Team as listed in CREDITS.txt          *
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
/// \author Sebastian Albers <s.albers@uni-muenster.de>
///

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include <pvsutil/Logger.h>
#include <pvsutil/Timer.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/AllPairs.h>
#include <SkelCL/Zip.h>
#include <SkelCL/Reduce.h>

using namespace skelcl;

const double epsilon = 1e-12;

template <typename ForwardIterator>
void init(ForwardIterator begin, ForwardIterator end)
{
  srand( (unsigned)time(0) );
  while (begin != end) {
    *begin = (int) ( ( (float)rand()/(float)RAND_MAX ) * 125 );
    ++begin;
  }
}

template<typename T>
bool isEqual(T lhs, T rhs) {
  return fabs(lhs - rhs) < epsilon;
}


// multiply two matrices
template<typename T>
double matrixMult(const int rowCountA, const int columnCountA, const int columnCountB, const int checkResult,
                  const std::string& zipFunc, const std::string& reduceFunc) {
  
  const int rowCountB = columnCountA;
  LOG_INFO("started: multiplication of matrices A (", rowCountA, " x ", columnCountA, ") and B (",
           rowCountB, " x ", columnCountB, ")");
  
  pvsutil::Timer timer;

  // initialize skeletons
  Zip<T(T, T)> zip(zipFunc);
  Reduce<T(T)> reduce(reduceFunc);
  AllPairs<T(T, T)> allpairs(reduce, zip);
  
  Matrix<T> left( {rowCountA, columnCountA} );
  Matrix<T> right( {rowCountB, columnCountB} );
  
  init(left.begin(), left.end());
  init(right.begin(), right.end());
  
  Matrix<T> output = allpairs(left, right);

  double elapsedTime = timer.stop();
  LOG_INFO("finished: matrix C (", output.rowCount(), " x ", output.columnCount(), ") calculated, ",
      "elapsed time: ", elapsedTime, " ms");

  if (checkResult) {
    unsigned int deviations = 0;
    for (size_t i = 0; i < output.rowCount(); ++i) {
      for (size_t j = 0; j < output.columnCount(); ++j) {
        T gold = 0;
        for (size_t k = 0; k < left.columnCount(); ++k) {
          gold += left[i][k] * right[k][j];
        }
        if (!isEqual(output[i][j], gold))
          ++deviations;
      }
    }

    if (deviations) {
      LOG_ERROR("result check failed: ", deviations, " deviations");
    } else {
      LOG_INFO("result check succeeded, no deviations");
    }
  }

  return elapsedTime;
}

double matrixMultFloat(const int rowCountA, const int columnCountA, const int columnCountB,
                       const int checkResult) {
  std::string zipFunc = "float func(float x, float y){ return x*y; }";
  std::string reduceFunc = "float func(float x, float y){ return x+y; }";
  return matrixMult<float>(rowCountA, columnCountA, columnCountB, checkResult, zipFunc, reduceFunc);
}


/*double matrixMultDouble(const int rowCountA, const int columnCountA, const int columnCountB,
                        const int checkResult) {
  std::string zipFunc = "double func(double x, double y){ return x*y; }";
  std::string reduceFunc = "double func(double x, double y){ return x+y; }";
  return matrixMult<double>(rowCountA, columnCountA, columnCountB, checkResult, zipFunc, reduceFunc);
}*/

int main(int argc, char* argv[])
{
//  pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
  pvsutil::defaultLogger.setOutput(std::cout);

  if (argc < 5 || argc > 8) {
    std::cout << "usage: matrix_mult <row_count_A> <column_count_A> <column_count_B>"
      << " <check_result> <repetitions> <device_type> <device_count>" << std::endl;
    std::cout << "- row_count_A: row count of left matrix" << std::endl;
    std::cout << "- column_count_A: column count of left matrix / row count"
      << "of right matrix" << std::endl;
    std::cout << "- column_count_B: column count of right matrix" << std::endl;
    std::cout << "- check_result: 0 = no check, 1 = result is checked" << std::endl;
    std::cout << "- repetitions: number of repetitions (optional, default: 1)" << std::endl;
    std::cout << "- device_type: ANY, CPU, GPU, ACCELERATOR (optional, default: ANY)" << std::endl;
    std::cout << "- device_count: number of devices (optional, default: 1)" << std::endl;
    return 1;
  }

  // parse command line arguments
  int rowCountA = atoi(argv[1]);
  int columnCountA = atoi(argv[2]);
  int columnCountB = atoi(argv[3]);
  int checkResult = atoi(argv[4]);
  int repetitions = 1;
  if (argc > 5) {
    repetitions = atoi(argv[5]);
  }
  device_type deviceType = device_type::ANY;
  if (argc > 6) {
    std::string deviceArg = argv[6];
    if (deviceArg == "CPU") {
        deviceType = device_type::CPU;
    }
    else if (deviceArg == "GPU") {
        deviceType = device_type::GPU;
    }
    else if (deviceArg == "ACCELERATOR") {
        deviceType = device_type::ACCELERATOR;
    }
  }
  int deviceCount = 1;
  if (argc > 7) {
    deviceCount = atoi(argv[7]);
  }

  init(nDevices(deviceCount).deviceType(deviceType)); // initialize SkelCL
  double totalTime = 0.0;
  for (int i = 0; i < repetitions; i++) {
    totalTime += matrixMultFloat(rowCountA, columnCountA, columnCountB, checkResult);
  }
  double avgTime = totalTime / repetitions;
  LOG_INFO("sizes: ", rowCountA, ", ", columnCountA, ", ", columnCountB, "; ",
      "average time: ", avgTime, " ms");

  return 0;
}

