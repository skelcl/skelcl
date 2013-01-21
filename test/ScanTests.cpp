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

#include <gtest/gtest.h>

#include <fstream>

#include <cstdio>

#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/Scan.h>

class ScanTest : public ::testing::Test {
protected:
  ScanTest() {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo );

    skelcl::init(skelcl::nDevices(1));
  };

  ~ScanTest() {
    skelcl::terminate();
  };
};

TEST_F(ScanTest, CreateScanWithString) {
  skelcl::Scan<float(float)> s{ "float func(float x, float y){ return x+y; }",
                                "0.0f" };
}

TEST_F(ScanTest, SimpleScan) {
  skelcl::Scan<int(int)> s{ "int func(int x, int y){ return x+y; }" };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = 1;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = s(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(i, output[i]);
  }
}

