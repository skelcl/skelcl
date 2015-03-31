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

long long getTime() {
  auto time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
}

template <typename ForwardIterator>
void init(ForwardIterator begin, ForwardIterator end) {
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
void matrixMult(const size_t rowCountA, const size_t columnCountA,
                const size_t columnCountB, const bool checkResult,
                const std::string& zipFunc, const std::string& reduceFunc,
                const std::string& func) {
  size_t rowCountB = columnCountA;
  std::string suffix = detail::util::typeToString<T>();

  AllPairs<T(T, T)> *allpairs;

  if (func.length() == 0) {
    Zip<T(T, T)> zip(zipFunc);
    Reduce<T(T)> reduce(reduceFunc);

    allpairs = new AllPairs<T(T, T)>(reduce, zip);
    suffix += "_zip-reduce";
  } else {
    allpairs = new AllPairs<T(T, T)>(func);

    suffix += "_generic";
  }

  Matrix<T> left( {rowCountA, columnCountA} );
  Matrix<T> right( {rowCountB, columnCountB} );

  init(left.begin(), left.end());
  init(right.begin(), right.end());

  auto start = getTime();

  Matrix<T> output = (*allpairs)(left, right);

  output.copyDataToHost();

  auto end = getTime();

  if (checkResult) {
    unsigned int deviations = 0;

    for (size_t i = 0; i < output.rowCount(); ++i) {
      for (size_t j = 0; j < output.columnCount(); ++j) {
        T gold = 0;

        for (size_t k = 0; k < left.columnCount(); ++k)
          gold += left[i][k] * right[k][j];

        if (!isEqual(output[i][j], gold))
          deviations += 1;
      }
    }

    if (deviations) {
      LOG_ERROR("Result check failed: ", deviations, " deviations");
      ABORT_WITH_ERROR("Expected result does not match computed result.");
    }
  }

  delete allpairs;

  printf("Elapsed time: %lld ms\n", end - start);
}

void matrixMultFloat(const size_t rowCountA, const size_t columnCountA,
                     const size_t columnCountB, const bool checkResult)
{
  std::string zipFunc = "float func(float x, float y){ return x*y; }";
  std::string reduceFunc = "float func(float x, float y){ return x+y; }";

  matrixMult<float>(rowCountA, columnCountA, columnCountB, checkResult,
                    zipFunc, reduceFunc, "");
}

void matrixMultFloatGeneric(const size_t rowCountA, const size_t columnCountA,
                            const size_t columnCountB, const bool checkResult)
{
  std::string func = "float func(lmatrix_t* row, rmatrix_t* col, "\
      "const unsigned int dim) {"                                 \
      "float res = 0;"                                            \
      "for (int i = 0; i < dim; ++i) {"                           \
      "  res += getElementFromRow(row, i) "                       \
      "* getElementFromColumn(col, i); }"                         \
      "return res;"                                               \
      "}";

  matrixMult<float>(rowCountA, columnCountA, columnCountB, checkResult,
                    "", "", func);
}

void matrixMultDouble(const size_t rowCountA, const size_t columnCountA,
                      const size_t columnCountB, const bool checkResult)
{
  std::string zipFunc = "double func(double x, double y){ return x*y; }";
  std::string reduceFunc = "double func(double x, double y){ return x+y; }";

  matrixMult<double>(rowCountA, columnCountA, columnCountB, checkResult,
                     zipFunc, reduceFunc, "");
}

void matrixMultDoubleGeneric(const size_t rowCountA, const size_t columnCountA,
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

  matrixMult<double>(rowCountA, columnCountA, columnCountB, checkResult,
                     "", "", func);
}


int main(int argc, char* argv[])
{
  // pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
  pvsutil::defaultLogger.setOutput(std::cout);

  using namespace pvsutil::cmdline;
  pvsutil::CLArgParser cmd(Description("Multiplication of two matrices"));

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


  auto rowCountA = Arg<size_t>(Flags(Short('d'), Long("row-count-a")),
                               Description("Row count of left matrix"),
                               Default<size_t>(2048));

  auto colCountA = Arg<size_t>(Flags(Short('m'), Long("col-count-a")),
                               Description("Column count of left matrix"),
                               Default<size_t>(2048));

  auto colCountB = Arg<size_t>(Flags(Short('n'), Long("col-count-b")),
                               Description("Column count of right matrix"),
                               Default<size_t>(4096));

  auto checkResult = Arg<bool>(Flags(Short('c'), Long("check")),
                               Description("Check the computed result "
                                           "against a sequential computed "
                                           "version."),
                               Default(false));

  auto iterations = Arg<size_t>(Flags(Short('i'), Long("iterations")),
                                Description("Number of iterations"),
                                Default<size_t>(5));

  auto useDoublePrecision = Arg<bool>(Flags(Long("double")),
                                      Description("Use double precision "
                                                  "data type"),
                                      Default(false));

  auto useGenericKernel = Arg<bool>(Flags(Long("generic-kernel")),
                                Description("Use generic kernel"),
                                Default(false));

  cmd.add(&verbose, &deviceType, &deviceCount, &rowCountA, &colCountA,
          &colCountB, &checkResult, &iterations, &deviceType, &deviceCount,
          &useDoublePrecision, &useGenericKernel);
  cmd.parse(argc, argv);

  if (verbose) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  init(nDevices(deviceCount).deviceType(deviceType));

  for (size_t i = 0; i < iterations; i++) {
    if (useGenericKernel && useDoublePrecision) {
      matrixMultDoubleGeneric(rowCountA, colCountA, colCountB, checkResult);
    } else if (useGenericKernel) { // && !useDoublePrecision
      matrixMultFloatGeneric(rowCountA, colCountA, colCountB, checkResult);
    } else if (useDoublePrecision) { // && !useGenericKernel
      matrixMultDouble(rowCountA, colCountA, colCountB, checkResult);
    } else { // !useDoublePrecision && !useGenericKernel
      matrixMultFloat(rowCountA, colCountA, colCountB, checkResult);
    }
  }

  terminate();

  printf("Matrix sizes: [%lu x %lu] [%lu x %lu]\n",
         rowCountA.getValue(), colCountA.getValue(),
         colCountA.getValue(), colCountB.getValue());

  return 0;
}
