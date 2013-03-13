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
#include <SkelCL/IndexVector.h>
#include <SkelCL/Map.h>

#include "Test.h"

class IndexVectorTest : public ::testing::Test {
protected:
  IndexVectorTest() {
    pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Debug);
    skelcl::init(skelcl::nDevices(1));
  };

  ~IndexVectorTest() {
    skelcl::terminate();
  };
};

TEST_F(IndexVectorTest, CreateIndexVector) {
  skelcl::Vector<skelcl::Index> vi(1024);

  EXPECT_EQ(1024, vi.size());
  EXPECT_EQ(0, vi.front());
  EXPECT_EQ(1023, vi.back());
}

TEST_F(IndexVectorTest, SimpleVoidMap) {
  skelcl::IndexVector index(1024);
  skelcl::Map<void(skelcl::Index)> m("void func(Index i, __global int* out) { out[i] = i; }");
  EXPECT_EQ(1024, index.size());

  skelcl::Vector<int> v(1024);

  m(index, skelcl::out(v));

  EXPECT_EQ(index.size(), v.size());
  for (size_t i = 0; i < v.size(); ++i) {
    EXPECT_EQ(i, v[i]);
  }
}

TEST_F(IndexVectorTest, SimpleMap) {
  skelcl::IndexVector index(1023);
  skelcl::Map<int(skelcl::Index)> m("int func(Index i) { return i; }");
  EXPECT_EQ(1023, index.size());

  auto v = m(index);

  EXPECT_EQ(index.size(), v.size());
  for (size_t i = 0; i < v.size(); ++i) {
    EXPECT_EQ(i, v[i]);
  }
}

