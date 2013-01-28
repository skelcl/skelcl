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

#include "Test.h"

class ReduceTest : public ::testing::Test {
protected:
  ReduceTest() {
    //pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Debug);
    skelcl::init(skelcl::nDevices(1));
  };

  ~ReduceTest() {
    skelcl::terminate();
  };
};

TEST_F(ReduceTest, CreateReduce) {
  skelcl::Reduce<float(float)> r("float func(float x, float y){ return x+y; }");
}

TEST_F(ReduceTest, SimpleReduce) {
  skelcl::Reduce<float(float)> r("float func(float x, float y){ return x+y; }");

  skelcl::Vector<float> input(1024);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }

  skelcl::Vector<float> output = r(input);

  EXPECT_LE(1, output.size());
  EXPECT_EQ(523776, output[0]);
}

TEST_F(ReduceTest, SimpleReduce2) {
  skelcl::Reduce<int(int)> r("int func(int x, int y){ return x+y; }");

  skelcl::Vector<int> input(1587);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }

  skelcl::Vector<int> output = r(input);

  EXPECT_LE(1, output.size());
  EXPECT_EQ(1258491, output[0]);
}

