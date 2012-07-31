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

// testing framework header
#include <gtest/gtest.h>
// std header
#include <fstream>
#include <iostream>
#include <cstdio>
// ssedit header
#include <ssedit/SourceFile.h>
#include <ssedit/Function.h>
#include <ssedit/Type.h>

class ParameterInfoTest : public ::testing::Test {
protected:
  ParameterInfoTest()
    : _testFileName("TestSource.cl"),
      _input(
R"(int foo(float f, const char c, int* ip, const char carr[]) {
  return f + c + *ip + carr[0] + carr[1];
})") {
    std::ofstream testFile(_testFileName, std::ios_base::trunc);
    testFile.write(_input.c_str(), _input.size());
  };

  ~ParameterInfoTest() {
    remove(_testFileName.c_str());
  };

  std::string _testFileName;
  std::string _input;
};

TEST_F(ParameterInfoTest, BasicParametersInfo) {
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Function foo(sourceFile.findFunction("foo"));

  EXPECT_EQ(4, foo.getParameters().size());
  EXPECT_EQ("f", foo.getParameters()[0].getName());
  EXPECT_EQ("c", foo.getParameters()[1].getName());
  EXPECT_EQ("ip", foo.getParameters()[2].getName());
  EXPECT_EQ("carr", foo.getParameters()[3].getName());
}

TEST_F(ParameterInfoTest, ParameterTypeInfo) {
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Function foo(sourceFile.findFunction("foo"));

  ssedit::Parameter first(foo.getParameters()[0]);
  EXPECT_EQ("Float", first.getType().getKindSpelling());
  EXPECT_EQ("Float", first.getType().getCanonicalType().getKindSpelling());
  EXPECT_EQ("Invalid", first.getType().getPointeeType().getKindSpelling());
  EXPECT_TRUE (first.getType().isPOD());
  EXPECT_FALSE(first.getType().isPointer());
  EXPECT_FALSE(first.getType().isConstantQualified());

  ssedit::Parameter second(foo.getParameters()[1]);
  EXPECT_EQ("Char_S", second.getType().getKindSpelling());
  EXPECT_EQ("Char_S", second.getType().getCanonicalType().getKindSpelling());
  EXPECT_EQ("Invalid", second.getType().getPointeeType().getKindSpelling());
  EXPECT_TRUE(second.getType().isPOD());
  EXPECT_FALSE(second.getType().isPointer());
  EXPECT_TRUE(second.getType().isConstantQualified());

  ssedit::Parameter third(foo.getParameters()[2]);
  EXPECT_EQ("Pointer", third.getType().getKindSpelling());
  EXPECT_EQ("Pointer", third.getType().getCanonicalType().getKindSpelling());
  EXPECT_EQ("Int", third.getType().getPointeeType().getKindSpelling());
  EXPECT_TRUE (third.getType().isPOD());
  EXPECT_TRUE (third.getType().isPointer());
  EXPECT_FALSE(third.getType().isConstantQualified());

  ssedit::Parameter fourth(foo.getParameters()[3]);
  EXPECT_EQ("Pointer", fourth.getType().getKindSpelling());
  EXPECT_EQ("Pointer", fourth.getType().getCanonicalType().getKindSpelling());
  EXPECT_EQ("Char_S", fourth.getType().getPointeeType().getKindSpelling());
  EXPECT_TRUE (fourth.getType().isPOD());
  EXPECT_TRUE (fourth.getType().isPointer());
  EXPECT_FALSE(fourth.getType().isConstantQualified());
}

