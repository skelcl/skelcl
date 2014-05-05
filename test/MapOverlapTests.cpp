/*****************************************************************************
 * Copyright (c) 2011-2012 The skelcl Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of skelcl.                                              *
 * skelcl is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * skelcl is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * skelcl is distributed in the hope that it will be useful,                 *
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
#include <cstdio>

#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Matrix.h>
#include <SkelCL/Vector.h>
#include <SkelCL/MapOverlap.h>

#include "Test.h"

class MapOverlapTest : public ::testing::Test {
protected:
  MapOverlapTest() {
    //pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Debug);
    skelcl::init(skelcl::nDevices(1));
  };

  ~MapOverlapTest() {
    skelcl::terminate();
  };
};

TEST_F(MapOverlapTest, CreateMapOverlapWithString) {
  skelcl::MapOverlap<float(float)> m {"float func(__local float* f) \
    { return -f[0]; }", 1};
}

TEST_F(MapOverlapTest, SimpleMapOverlap) {
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[0]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(input[i], output[i]);
  }
}

TEST_F(MapOverlapTest, SimpleMapOverlap2) {
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[-1]+f[0]+f[1]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  EXPECT_EQ(0+input[0]+input[1], output[0]);
  for (size_t i = 1; i < output.size() - 1; ++i) {
    EXPECT_EQ(input[i-1]+input[i]+input[i+1], output[i]);
  }
  EXPECT_EQ(input[input.size()-2]+input[input.size()-1]+0, output.back());
}

TEST_F(MapOverlapTest, SimpleMapOverlapWithZeroOverlap) {
  skelcl::MapOverlap<float(float)> m{
    "float func(__local float* f){ return -f[0]; }", 0 };

  skelcl::Vector<float> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i * 2.5f;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<float> output = m(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(-input[i], output[i]);
  }
}

TEST_F(MapOverlapTest, SimpleMultiDeviceMapOverlap) {
  skelcl::terminate();
  skelcl::init(skelcl::nDevices(2));
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[0]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(input[i], output[i]);
  }
}

TEST_F(MapOverlapTest, SimpleMultiDeviceMapOverlap2) {
  skelcl::terminate();
  skelcl::init(skelcl::nDevices(2));
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[-1] + f[0] + f[+1]; }", 1 };

  skelcl::Vector<int> input(499);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(499, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(499, output.size());
  EXPECT_EQ(0+input[0]+input[1], output[0]);
  for (size_t i = 1; i < output.size() - 1; ++i) {
    EXPECT_EQ(input[i-1]+input[i]+input[i+1], output[i]);
  }
  EXPECT_EQ(input[input.size()-2]+input[input.size()-1]+0, output.back());
}

TEST_F(MapOverlapTest, SimpleMatrixMapOverlap) {
//  pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
  skelcl::terminate();
  skelcl::init(skelcl::nDevices(1));
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[0]; }", 1 };

  skelcl::Matrix<int> input( skelcl::MatrixSize{10, 10} );
  for (size_t i = 0; i < input.size().columnCount(); ++i) {
    for (size_t j = 0; j < input.size().rowCount(); ++j) {
      input[i][j] = i*j;
      LOG_DEBUG("input[", i, "][", j, "] = ", i*j);
    }
  }
  EXPECT_EQ(10, input.size().rowCount());
  EXPECT_EQ(10, input.size().columnCount());

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(10, output.size().rowCount());
  EXPECT_EQ(10, output.size().columnCount());
  for (size_t i = 0; i < output.size().columnCount(); ++i) {
    for (size_t j = 0; j < output.size().rowCount(); ++j) {
      LOG_DEBUG("input[", i, "][", j, "] = ", i*j);
      EXPECT_EQ(input[i][j], output[i][j]);
    }
  }
}

#if 0
TEST_F(MapOverlapTest, SimpleMatrixMapOverlap2) {
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[-1]+f[0]+f[1]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  EXPECT_EQ(0+input[0]+input[1], output[0]);
  for (size_t i = 1; i < output.size() - 1; ++i) {
    EXPECT_EQ(input[i-1]+input[i]+input[i+1], output[i]);
  }
  EXPECT_EQ(input[input.size()-2]+input[input.size()-1]+0, output.back());
}

TEST_F(MapOverlapTest, SimpleMatrixMultiDeviceMapOverlap) {
  skelcl::terminate();
  skelcl::init(2);
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[0]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(input[i], output[i]);
  }
}

TEST_F(MapOverlapTest, SimpleMatrixMultiDeviceMapOverlap2) {
  skelcl::terminate();
  skelcl::init(2);
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[-1] + f[0] + f[+1]; }", 1 };

  skelcl::Vector<int> input(499);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(499, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(499, output.size());
  EXPECT_EQ(0+input[0]+input[1], output[0]);
  for (size_t i = 1; i < output.size() - 1; ++i) {
    EXPECT_EQ(input[i-1]+input[i]+input[i+1], output[i]);
  }
  EXPECT_EQ(input[input.size()-2]+input[input.size()-1]+0, output.back());
}
#endif
