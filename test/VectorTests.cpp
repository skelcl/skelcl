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
#include <SkelCL/Vector.h>

#include "Test.h"
/// \cond
/// Don't show this test in doxygen

class VectorTest : public ::testing::Test {
protected:
  VectorTest() {
    pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Debug);
    skelcl::init(skelcl::nDevices(1));
  }

  ~VectorTest() {
    skelcl::terminate();
  }
};

struct A {
  A() : _a(0) {}
  A(int a) : _a(a) {}
  int _a;
};

TEST_F(VectorTest, CreateEmptyVector) {
  skelcl::Vector<int> vi;

  EXPECT_TRUE(vi.empty());
  EXPECT_EQ(0, vi.size());
}

TEST_F(VectorTest, Copy) {
  skelcl::Vector<int> vi(5);
  for(unsigned i = 0; i < 5; ++i) {
    vi[i] = 5;
  }

  EXPECT_FALSE(vi.empty());
  EXPECT_EQ(5, vi.size());
  for (unsigned i = 0; i < 5; ++i) {
    EXPECT_EQ(5, vi[i]);
  }

  skelcl::Vector<int> copy(vi);

  for (unsigned i = 0; i < 5; ++i) {
    vi[i] = 0;
  }

  EXPECT_FALSE(copy.empty());
  EXPECT_EQ(vi.size(), copy.size());
  for (unsigned i = 0; i < vi.size(); ++i) {
    EXPECT_EQ(5, copy[i]);
  }
}

TEST_F(VectorTest, CopyWithDataOnDevice) {
  skelcl::Vector<int> vi(5);
  for(unsigned i = 0; i < 5; ++i) {
    vi[i] = 5;
  }
  vi.setDistribution(skelcl::detail::SingleDistribution< skelcl::Vector<int> >());
  vi.createDeviceBuffers();
  vi.copyDataToDevices();

  EXPECT_FALSE(vi.empty());
  EXPECT_EQ(5, vi.size());
  for (unsigned i = 0; i < 5; ++i) {
    EXPECT_EQ(5, vi[i]);
  }

  vi.dataOnDeviceModified(); // fake modification on the device

  skelcl::Vector<int> copy(vi);

  for (unsigned i = 0; i < 5; ++i) {
    vi[i] = 0;
  }

  EXPECT_FALSE(copy.empty());
  EXPECT_EQ(vi.size(), copy.size());
  for (unsigned i = 0; i < vi.size(); ++i) {
    EXPECT_EQ(5, copy[i]);
  }
}

TEST_F(VectorTest, CreateVector) {
  skelcl::Vector<int> vi(10);

  EXPECT_FALSE(vi.empty());
  EXPECT_EQ(10, vi.size());
  EXPECT_EQ(0, *vi.begin());
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(0, vi[i]);
  }

  skelcl::Vector<A> va(5, A(1));

  EXPECT_FALSE(va.empty());
  EXPECT_EQ(5, va.size());
  EXPECT_EQ(1, va.begin()->_a);
}

TEST_F(VectorTest, Resize) {
  skelcl::Vector<int> vi;
  vi.resize(10);
  EXPECT_EQ(10, vi.size());

  vi.push_back(5);
  EXPECT_EQ(11, vi.size());

  vi.pop_back();
  EXPECT_EQ(10, vi.size());

  vi.insert( vi.begin(), 5 );
  EXPECT_EQ(11, vi.size());

  vi.insert( vi.begin(), 2, 5 );
  EXPECT_EQ(13, vi.size());

  std::vector<int> other(2);
  vi.insert( vi.begin(), other.begin(), other.end() );
  EXPECT_EQ(15, vi.size());

  vi.erase( vi.begin() );
  EXPECT_EQ(14, vi.size());

  vi.erase( vi.begin(), vi.begin()+2 );
  EXPECT_EQ(12, vi.size());

  vi.clear();
  EXPECT_EQ(0, vi.size());
}

typedef struct Position {
  Position() : _x(0), _y(0) {}
  Position(int x, int y) : _x(x), _y(y) {}
  int _x;
  int _y;
} Position;

TEST_F(VectorTest, CompleteInitialization) {
  skelcl::Vector<Position> positions(1024*768);

  auto inserter = positions.begin();
  for (int x = 0; x < 1024; ++x)
    for (int y = 0; y < 768; ++y)
      *(inserter++) = Position(x,y);

  EXPECT_EQ(1024*768, positions.size());

  EXPECT_EQ(23,   positions[23*768+42]._x);
  EXPECT_EQ(42,   positions[23*768+42]._y);
}

/// \endcond

