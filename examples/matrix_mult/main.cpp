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

#include <SkelCL/detail/Util.h>

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
double matrixMult(const int rowCountA, const int columnCountA, 
                  const int columnCountB, const int checkResult,
                  const std::string& zipFunc, const std::string& reduceFunc, 
                  const std::string& func, const std::string& deviceType, 
                  int deviceCount) {
  
  const int rowCountB = columnCountA;
  LOG_INFO("started: multiplication of matrices A (", rowCountA, " x ", 
           columnCountA, ") and B (", rowCountB, " x ", columnCountB, ")");
  
  pvsutil::Timer timer;

  // initialize skeletons
  std::string suffix = detail::util::typeToString<T>();
  AllPairs<T(T, T)> *allpairs;
  if (func.length() == 0) {
    Zip<T(T, T)> zip(zipFunc);
    Reduce<T(T)> reduce(reduceFunc);
    allpairs = new AllPairs<T(T, T)>(reduce, zip);
    suffix += "_zip-reduce";
  } else {
    allpairs = new AllPairs<T(T, T)>(func);
    suffix += "_alternative";
  }

  Matrix<T> left( {rowCountA, columnCountA} );
  Matrix<T> right( {rowCountB, columnCountB} );
  
  init(left.begin(), left.end());
  init(right.begin(), right.end());
  
  Matrix<T> output = (*allpairs)(left, right);
  
  output.copyDataToHost();
  double elapsedTime = timer.stop();
  
  printf("|||SkelCL_allpairs_%s;%d;%d;%d;%s;%d;%f\n", suffix.c_str(), rowCountA, 
    columnCountA, columnCountB, deviceType.c_str(), deviceCount, elapsedTime);
  LOG_INFO("finished: matrix C (", output.rowCount(), " x ", 
           output.columnCount(), ") calculated, ", "elapsed time: ", 
           elapsedTime, " ms");

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

  delete allpairs;
  return elapsedTime;
}

double matrixMultFloat(const int rowCountA, const int columnCountA, 
                       const int columnCountB, const int checkResult, 
                       const std::string& deviceType, int deviceCount) {
  std::string zipFunc = "float func(float x, float y){ return x*y; }";
  std::string reduceFunc = "float func(float x, float y){ return x+y; }";
  return matrixMult<float>(rowCountA, columnCountA, columnCountB, checkResult, 
                           zipFunc, reduceFunc, "", deviceType, deviceCount);
}

double matrixMultFloatAlt(const int rowCountA, const int columnCountA, 
                          const int columnCountB, const int checkResult, 
                          const std::string& deviceType, int deviceCount) {
  std::string func = "float func(lmatrix_t* row, rmatrix_t* col, "\
                                "const unsigned int dim) {" \
                        "float res = 0;" \
                        "for (int i = 0; i < dim; ++i) {" \
                        "  res += getElementFromRow(row, i) "\
                               "* getElementFromColumn(col, i); }" \
                        "return res;" \
                      "}";
  return matrixMult<float>(rowCountA, columnCountA, columnCountB, checkResult, 
                           "", "", func, deviceType, deviceCount);
}

double matrixMultDouble(const int rowCountA, const int columnCountA, 
                        const int columnCountB, const int checkResult, 
                        const std::string& deviceType, int deviceCount) {
  std::string zipFunc = "double func(double x, double y){ return x*y; }";
  std::string reduceFunc = "double func(double x, double y){ return x+y; }";
  return matrixMult<double>(rowCountA, columnCountA, columnCountB, checkResult,
                            zipFunc, reduceFunc, "", deviceType, deviceCount);
}

double matrixMultDoubleAlt(const int rowCountA, const int columnCountA, 
                           const int columnCountB, const int checkResult, 
                           const std::string& deviceType, int deviceCount) {
  std::string func = "double func(lmatrix_t* row, rmatrix_t* col, "\
                                 "const unsigned int dim) {" \
                        "double res = 0;" \
                        "for (int i = 0; i < dim; ++i) {" \
                        "  res += getElementFromRow(row, i) "\
                               "* getElementFromColumn(col, i); }" \
                        "return res;" \
                      "}";
  return matrixMult<double>(rowCountA, columnCountA, columnCountB, checkResult, 
                            "", "", func, deviceType, deviceCount);
}


int main(int argc, char* argv[])
{
  // pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
  pvsutil::defaultLogger.setOutput(std::cout);

  if (argc < 5 || argc > 10) {
    std::cout << "usage: matrix_mult <row_count_A> <column_count_A> " \
                                    "<column_count_B>" << std::endl
      << "                   <check_result> <repetitions> <device_type>" 
      << std::endl
      << "                   [[<device_count> [<double_precision>] " \
                            "[<alt-kernel>]]]" << std::endl
      << "- row_count_A: row count of left matrix" << std::endl
      << "- column_count_A: column count of left matrix / row count of right " \
                          "matrix" << std::endl
      << "- column_count_B: column count of right matrix" << std::endl
      << "- check_result: 0 = no check, 1 = result is checked" << std::endl
      << "- repetitions: number of repetitions (optional, default: 1)" 
      << std::endl
      << "- device_type: ANY, CPU, GPU, ACCELERATOR (optional, default: ANY)" 
      << std::endl
      << "- device_count: number of devices (optional, default: 1)" << std::endl
      << "- double_precision: use double precision, 0 or 1 (optional, " \
                             "default: 0)" << std::endl
      << "- alt_kernel: use alternative kernel (instead of Zip and Reduce), " \
                       "0 or 1" << std::endl
      << "              (optional, default: 0)" << std::endl;
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
  std::string deviceArg = "Any";
  if (argc > 6) {
    deviceArg = argv[6];
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
  bool double_precicsion = false;
  if (argc > 8) {
    double_precicsion = atoi(argv[8]);
  }
  bool alt_kernel = false;
  if (argc > 9) {
    alt_kernel = atoi(argv[9]);
  }

  init(nDevices(deviceCount).deviceType(deviceType)); // initialize SkelCL
  double totalTime = 0.0;
  for (int i = 0; i < repetitions; i++) {
    if (alt_kernel && double_precicsion) {
      totalTime += matrixMultDoubleAlt(rowCountA, columnCountA, columnCountB,
                                       checkResult, deviceArg, deviceCount);
    } else if (alt_kernel) { // && !double_precicsion
      totalTime += matrixMultFloatAlt(rowCountA, columnCountA, columnCountB,
                                      checkResult, deviceArg, deviceCount);
    } else if (double_precicsion) { // && !alt_kernel
      totalTime += matrixMultDouble(rowCountA, columnCountA, columnCountB,
                                    checkResult, deviceArg, deviceCount);
    } else { // !double_precicsion && !alt_kernel
      totalTime += matrixMultFloat(rowCountA, columnCountA, columnCountB, 
                                   checkResult, deviceArg, deviceCount);
    }
  }
  double avgTime = totalTime / repetitions;
  LOG_INFO("sizes: ", rowCountA, ", ", columnCountA, ", ", columnCountB, "; ",
      "average time: ", avgTime, " ms");

  return 0;
}

