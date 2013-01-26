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
#include <vector>

#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/AllPairs.h>
#include <SkelCL/Zip.h>
#include <SkelCL/Reduce.h>

using namespace skelcl;

template <typename ForwardIterator>
void init(ForwardIterator begin, ForwardIterator end)
{
  srand( (unsigned)time(0) );
  while (begin != end) {
    *begin = (int) ( ( (float)rand()/(float)RAND_MAX ) * 125 );
    ++begin;
  }
}

// multiply two matrices
void matrixMult(const int rowCountA, const int columnCountA, const int columnCountB) {
  
  const int rowCountB = columnCountA;
  LOG_INFO("started: multiplication of matrices A (", rowCountA, " x ", columnCountA, ") and B (",
           rowCountB, " x ", columnCountB, ")");
  
  // initialize skeletons
  Zip<float(float, float)> zip("float func(float x, float y){ return x*y; }");
  Reduce<float(float)> reduce("float func(float x, float y){ return x+y; }");
  AllPairs<float(float, float)> allpairs(reduce, zip);
  
  std::vector<float> vectorA(rowCountA * columnCountA);
  std::vector<float> vectorB(rowCountB * columnCountB);
  
  init(vectorA.begin(), vectorA.end());
  init(vectorB.begin(), vectorB.end());
  
  Matrix<float> matrixA(vectorA, columnCountA);
  Matrix<float> matrixB(vectorB, columnCountB);
  
  Matrix<float> output = allpairs(matrixA, matrixB);
  LOG_INFO("finished: matrix C (", output.rowCount(), " x ", output.columnCount(), ") calculated");
}

int main()
{
//  pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
  init(skelcl::nDevices(1)); // initialize SkelCL
  matrixMult(1024, 512, 2048);
  return 0;
}