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

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
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

int main()
{
  skelcl::init(1); // initialize SkelCL

  Zip<int(int,int)> mult("int func(int x, int y){ return x*y; }");
  Reduce<int(int)>    sum("int func(int x, int y){ return x+y; }", "0");

  Vector<int> A(1024);
  Vector<int> B(1024);

  init(A.begin(), A.end());
  init(B.begin(), B.end());

  // process gold:
  int res = 0;
  for (size_t i = 0; i < A.size(); ++i) {
    res += (A[i] * B[i]);
  }

  std::cout << "gold:   " << res << std::endl;

  Vector<int> C = sum( mult(A, B) );

  std::cout << "skelcl: " << C.front() << std::endl;

  return 0;
}

