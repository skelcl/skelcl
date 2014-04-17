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

#include <pvsutil/Logger.h>

#include <SkelCL/Distributions.h>
#include <SkelCL/SkelCL.h>
#include <SkelCL/IndexMatrix.h>
#include <SkelCL/Map.h>

#include "Test.h"

class IndexMatrixTest : public ::testing::Test {
protected:
  IndexMatrixTest() {
    //pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
    skelcl::init(skelcl::nDevices(1));
  }

  ~IndexMatrixTest() {
    skelcl::terminate();
  }
};

TEST_F(IndexMatrixTest, CreateIndexMatrix) {
  skelcl::Matrix<skelcl::IndexPoint> mi({256, 32});

  EXPECT_EQ(256*32, mi.size().elemCount());
  EXPECT_EQ(skelcl::IndexPoint(0,0), mi.front());
  EXPECT_EQ(skelcl::IndexPoint(255,31), mi.back());
}

TEST_F(IndexMatrixTest, SimpleVoidMap) {
  skelcl::IndexMatrix index({128, 32});
  skelcl::Map<void(skelcl::IndexPoint)> map(R"(

void func(IndexPoint ip, int_matrix_t out) { set(out, ip.y, ip.x, ip.y+ip.x); }
)");
  EXPECT_EQ(128*32, index.size().elemCount());

  skelcl::Matrix<int> m({128, 32});

  map(index, skelcl::out(m));

  for (size_t y = 0; y < m.size().rowCount(); ++y) {
    for (size_t x = 0; x < m.size().columnCount(); ++x) {
      EXPECT_EQ(y+x, m[y][x]);
    }
  }
}

TEST_F(IndexMatrixTest, SimpleMap) {
  skelcl::Map<int(skelcl::IndexPoint)> m("int func(IndexPoint i) { return i.y+i.x; }");
  
  skelcl::IndexMatrix index({32, 128});
  EXPECT_EQ(32*128, index.size().elemCount());

  skelcl::Matrix<int> out = m(index);
  
  EXPECT_EQ(out.size().rowCount(), index.size().rowCount());
  EXPECT_EQ(out.size().columnCount(), index.size().columnCount());

  for (size_t y = 0; y < out.size().rowCount(); ++y) {
    for (size_t x = 0; x < out.size().columnCount(); ++x) {
      EXPECT_EQ(y+x, out[y][x]);
    }
  }
}
