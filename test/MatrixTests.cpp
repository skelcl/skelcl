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

#include <SkelCL/Distributions.h>
#include <SkelCL/SkelCL.h>
#include <SkelCL/Matrix.h>
#include <SkelCL/detail/Logger.h>

class MatrixTest : public ::testing::Test {
protected:
  MatrixTest() {
    skelcl::detail::defaultLogger.setLoggingLevel(skelcl::detail::Logger::Severity::Debug);
    skelcl::detail::defaultLogger.setLoggingLevel(skelcl::detail::Logger::Severity::Debug);
    skelcl::init(skelcl::nDevices(1));
  };

  ~MatrixTest() {
    skelcl::terminate();
  };
};

TEST_F(MatrixTest, CreateEmptyMatrix) {
  skelcl::Matrix<int> mi;

  EXPECT_TRUE(mi.empty());
  EXPECT_EQ( skelcl::MatrixSize(0,0) , mi.size());
}

TEST_F(MatrixTest, CreateMatrix) {
  skelcl::Matrix<int> mi( {10,10} );

  EXPECT_FALSE(mi.empty());
  EXPECT_EQ(100, mi.size().elemCount());
  EXPECT_EQ(0, *mi.begin());
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      EXPECT_EQ(0, mi({i,j}));
    }
  }
}

TEST_F(MatrixTest, CreateMatrixFrom2DVector) {
  std::vector<std::vector<int> > vec(10);
  for (int i = 0; i < 10; ++i) {
    vec[i].resize(10);
    for (int j = 0; j < 10; ++j) {
      vec[i][j] = i*j;
    }
  }

  skelcl::Matrix<int> mi = skelcl::Matrix<int>::from2DVector(vec);

  EXPECT_FALSE(mi.empty());
  EXPECT_EQ(100, mi.size().elemCount());
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      EXPECT_EQ(i*j, mi[i][j]);
    }
  }

}

