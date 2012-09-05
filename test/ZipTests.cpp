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

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/Zip.h>
#include <SkelCL/detail/Logger.h>

class ZipTest : public ::testing::Test {
protected:
  ZipTest() {
//    skelcl::detail::Logger.setLoggingLevel(skelcl::detail::Severity::Debug);
    skelcl::init(skelcl::nDevices(1));
  };

  ~ZipTest() {
    skelcl::terminate();
  };
};

TEST_F(ZipTest, CreateZipWithString) {
  skelcl::Zip<float(float, float)> m("float func(float x, float y){ return x*y; }");
}

TEST_F(ZipTest, CreateZipWithFile) {
  std::string filename("ZipFunction.cl");
  {
    std::ofstream file(filename, std::ios_base::trunc);
    file << "float func(float x, float y) { return x+y; }";
  }
  std::ifstream file(filename);

  skelcl::Zip<float(float, float)> m(file);

  remove(filename.c_str()); // delete file
}

TEST_F(ZipTest, SimpleZip) {
  skelcl::Zip<float(float, float)> z("float func(float x, float y){ return x+y; }");

  skelcl::Vector<float> left(10);
  for (size_t i = 0; i < left.size(); ++i) {
    left[i] = i * 2.5f;
  }
  EXPECT_EQ(10, left.size());

  skelcl::Vector<float> right(10);
  for (size_t i = 0; i < right.size(); ++i) {
    right[i] = i * 7.5f;
  }
  EXPECT_EQ(10, right.size());

  auto output = z(left, right);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(left[i]+right[i], output[i]);
  }
}

TEST_F(ZipTest, AddArgs) {
  skelcl::Zip<float(float, float)> z("float func(float x, float y, float add, float add2)\
                                 { return (x*y)+add+add2; }");

  skelcl::Vector<float> left(10);
  for (size_t i = 0; i < left.size(); ++i) {
    left[i] = i * 2.5f;
  }
  EXPECT_EQ(10, left.size());

  skelcl::Vector<float> right(10);
  for (size_t i = 0; i < right.size(); ++i) {
    right[i] = i * 4.5f;
  }
  EXPECT_EQ(10, right.size());

  float add  = 5.25f;
  float add2 = 7.75f;

  auto output = z(left, right, add, add2);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ( (left[i]*right[i])+add+add2, output[i]);
  }
}

TEST_F(ZipTest, SimpleZip2D) {
  skelcl::Zip<float(float,float)> z(
    "float func(float mat1,float mat2)""{ return mat1 + mat2; }");

  std::vector<float> left(10);
  for (size_t i = 0; i < left.size(); ++i) {
    left[i] = i * 2.5f;
  }
  skelcl::Matrix<float> matrix_left(left, 3);
  EXPECT_EQ(skelcl::Matrix<float>::size_type(4,3), matrix_left.size());

  std::vector<float> right(10);
  for (size_t i = 0; i < right.size(); ++i) {
    right[i] = i * 7.5f;
  }
  skelcl::Matrix<float> matrix_right(right,3);
  EXPECT_EQ(skelcl::Matrix<float>::size_type(4,3), matrix_right.size());

  auto output = z(matrix_left, matrix_right);

  EXPECT_EQ(matrix_left.size(), output.size());

  for (size_t i = 0; i < output.rowCount(); ++i) {
    for(size_t j = 0; j < output.columnCount(); ++j) {
      EXPECT_EQ(matrix_left[i][j] + matrix_right[i][j], output[i][j]);
    }
  }
}

