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
#include <sstream>
#include <string>
#include <vector>

#include <pvsutil/Logger.h>
#include <pvsutil/Timer.h>
#include <pvsutil/CLArgParser.h>

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
double matrixMult(const size_t rowCountA, const size_t columnCountA,
                  const size_t columnCountB, const bool checkResult,
                  const std::string& zipFunc, const std::string& reduceFunc,
                  const std::string& func) {

  size_t rowCountB = columnCountA;
  LOG_INFO("started: multiplication of matrices A (", rowCountA, " x ",
           columnCountA, ") and B (", rowCountB, " x ", columnCountB, ")");

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

  pvsutil::Timer timer;

  Matrix<T> output = (*allpairs)(left, right);

  output.copyDataToHost();
  double elapsedTime = timer.stop();

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

double matrixMultFloat(const size_t rowCountA, const size_t columnCountA,
                       const size_t columnCountB, const bool checkResult)
{
  std::string zipFunc = "float func(float x, float y){ return x*y; }";
  std::string reduceFunc = "float func(float x, float y){ return x+y; }";
  return matrixMult<float>(rowCountA, columnCountA, columnCountB, checkResult,
                           zipFunc, reduceFunc, "");
}

double matrixMultFloatAlt(const size_t rowCountA, const size_t columnCountA,
                          const size_t columnCountB, const bool checkResult)
{
  std::string func = "float func(lmatrix_t* row, rmatrix_t* col, "\
                                "const unsigned int dim) {" \
                        "float res = 0;" \
                        "for (int i = 0; i < dim; ++i) {" \
                        "  res += getElementFromRow(row, i) "\
                               "* getElementFromColumn(col, i); }" \
                        "return res;" \
                      "}";
  return matrixMult<float>(rowCountA, columnCountA, columnCountB, checkResult,
                           "", "", func);
}

double matrixMultDouble(const size_t rowCountA, const size_t columnCountA,
                        const size_t columnCountB, const bool checkResult)
{
  std::string zipFunc = "double func(double x, double y){ return x*y; }";
  std::string reduceFunc = "double func(double x, double y){ return x+y; }";
  return matrixMult<double>(rowCountA, columnCountA, columnCountB, checkResult,
                            zipFunc, reduceFunc, "");
}

double matrixMultDoubleAlt(const size_t rowCountA, const size_t columnCountA,
                           const size_t columnCountB, const bool checkResult)
{
  std::string func = "double func(lmatrix_t* row, rmatrix_t* col, "\
                                 "const unsigned int dim) {" \
                        "double res = 0;" \
                        "for (int i = 0; i < dim; ++i) {" \
                        "  res += getElementFromRow(row, i) "\
                               "* getElementFromColumn(col, i); }" \
                        "return res;" \
                      "}";
  return matrixMult<double>(rowCountA, columnCountA, columnCountB, checkResult,
                            "", "", func);
}


int main(int argc, char* argv[])
{
  // pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
  pvsutil::defaultLogger.setOutput(std::cout);

  using namespace pvsutil::cmdline;
  pvsutil::CLArgParser cmd(Description("Matrix multiplication example "
                                       "in SkelCL"));

  auto rowCountA = Arg<size_t>(Flags(Short('d'), Long("row_count_A")),
                               Description("Row count of left matrix"));

  auto colCountA = Arg<size_t>(Flags(Short('m'), Long("col_count_A")),
                               Description("Column count of left matrix"));

  auto colCountB = Arg<size_t>(Flags(Short('n'), Long("col_count_B")),
                               Description("Column count of right matrix"));

  auto checkResult = Arg<bool>(Flags(Short('c'), Long("check_result")),
                               Description("Check result"),
                               Default(false));

  auto repetitions = Arg<size_t>(Flags(Short('r'), Long("repetitions")),
                                 Description("Number of repetitions"),
                                 Default<size_t>(1));

  auto deviceType = Arg<device_type>(Flags(Long("device_type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto deviceCount = Arg<size_t>(Flags(Long("device_count")),
                                 Description("Device Count"),
                                 Default<size_t>(1));

  auto useDoublePrecision = Arg<bool>(Flags(Long("use_double")),
                                      Description("Use double precision for "
                                                  "the computation"),
                                      Default(false));

  auto useAltKernel = Arg<bool>(Flags(Long("alt_kernel")),
                                Description("Use alternative kernel"),
                                Default(false));

  cmd.add(rowCountA, colCountA, colCountB, checkResult, repetitions,
          deviceType, deviceCount, useDoublePrecision, useAltKernel);

  cmd.parse(argc, argv);

  init(nDevices(deviceCount).deviceType(deviceType)); // initialize SkelCL
  double totalTime = 0.0;
  for (size_t i = 0; i < repetitions; i++) {
    if (useAltKernel && useDoublePrecision) {
      totalTime +=
          matrixMultDoubleAlt(rowCountA, colCountA, colCountB, checkResult);
    } else if (useAltKernel) { // && !useDoublePrecision
      totalTime +=
          matrixMultFloatAlt(rowCountA, colCountA, colCountB, checkResult);
    } else if (useDoublePrecision) { // && !useAltKernel
      totalTime +=
          matrixMultDouble(rowCountA, colCountA, colCountB, checkResult);
    } else { // !useDoublePrecision && !useAltKernel
      totalTime +=
          matrixMultFloat(rowCountA, colCountA, colCountB, checkResult);
    }
  }
  double avgTime = totalTime / repetitions;
  LOG_INFO("sizes: ", rowCountA, ", ", colCountA, ", ", colCountB, "; ",
      "average time: ", avgTime, " ms");

  return 0;
}
