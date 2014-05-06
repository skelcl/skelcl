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

#include <fstream>

#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/Reduce.h>

#include <iostream>

#include "Test.h"

class ReduceTest : public ::testing::Test {
protected:
  ReduceTest() {
    //pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Debug);
    skelcl::init(skelcl::nDevices(1));
  }

  ~ReduceTest() {
    skelcl::terminate();
  }
};

TEST_F(ReduceTest, CreateReduce) {
  skelcl::Reduce<float(float)> r("float func(float x, float y){ return x+y; }");
}


TEST_F(ReduceTest, SimpleReduce) {
  skelcl::Reduce<float(float)> r("float func(float x, float y){ return x+y; }");

  skelcl::Vector<float> input(100);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }

  skelcl::Vector<float> output = r(input);

  EXPECT_LE(1,    output.size());
  EXPECT_EQ(4950, output[0]);
}


TEST_F(ReduceTest, SimpleReduce2) {
  skelcl::Reduce<int(int)> r("int func(int x, int y){ return x+y; }");

  skelcl::Vector<int> input(1587);
  for (unsigned int i = 0; i < input.size(); ++i) {
    input[i] = i;
  }

  skelcl::Vector<int> output = r(input);

  EXPECT_LE(1, output.size());
  EXPECT_EQ(1258491, output[0]);
}



TEST_F(ReduceTest, LongReduce) {
  skelcl::Reduce<int(int)> r("int func(int x, int y){ return x+y; }");

  skelcl::Vector<int> input(100000000);
  for (unsigned int i = 0; i < input.size(); ++i) {
    input[i] = 1;
  }

  skelcl::Vector<int> output = r(input);

  EXPECT_LE(1, output.size());
  EXPECT_EQ(100000000, output[0]);
}



TEST_F(ReduceTest, nSizesReduce1) {
  skelcl::Reduce<float(float)> r("float func(float x, float y){ return x+y; }");

  const int N = 256;

  skelcl::Vector<float> input;
  skelcl::Vector<float> output;

  for( int i = 1; i < N; ++i )
  {
    input.resize(i, 1);
    output = r(input);
   
    EXPECT_EQ(input.size(), output[0]);
  }
}


TEST_F(ReduceTest, nSizesReduce2) {
    skelcl::Reduce<float(float)> r("float func(float x, float y){ return x+y; }");
  
    const int N = 256;
  
    skelcl::Vector<float> input;
    skelcl::Vector<float> output;
  
    for( int i = 8192; i < (8192 + N); ++i )
    {
      input.resize(i, 1);
      output = r(input);
      
      EXPECT_EQ(input.size(), output[0]);
    }
 }


