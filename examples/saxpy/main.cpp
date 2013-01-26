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

#include <pvsutil/Logger.h>
#include <pvsutil/Timer.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/Zip.h>

using namespace skelcl;

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

int main()
{
  int SIZE = 1024 * 1024; // 1 MB
  skelcl::init(); // initialize SkelCL

  pvsutil::Timer timer;
  
  // Y <- a * X + Y
  Zip<float(float, float)> saxpy("float func(float x, float y, float a){ return a*x + y; }");

  Vector<float> X(SIZE); fillVector(X.begin(), X.end());
  Vector<float> Y(SIZE); fillVector(Y.begin(), Y.end());
  float a = fillScalar();

  Y = saxpy( X, Y, a );

  pvsutil::Timer::time_type time = timer.stop();
  
  std::cout << "Y accumulated: ";
  std::cout << std::accumulate(Y.begin(), Y.end(), 0.0f) << std::endl;
  std::cout << "elapsed time: " << time << " ms" << std::endl;

  return 0;
}

