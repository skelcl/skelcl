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

// std header
#include <fstream>
#include <iostream>
#include <cstdio>
// ssedit header
#include <ssedit/SourceFile.h>
#include <ssedit/Function.h>
#include <ssedit/Type.h>

#include "Test.h"

class FunctionInfoTest : public ::testing::Test {
protected:
  FunctionInfoTest()
    : _testFileName("TestSource.cpp"),
      _input(
R"(int foo(float f, const char* c) {
  return f + c;
})") {
    std::ofstream testFile(_testFileName, std::ios_base::trunc);
    testFile.write(_input.c_str(), _input.size());
  };

  ~FunctionInfoTest() {
    remove(_testFileName.c_str());
  };

  std::string _testFileName;
  std::string _input;
};

TEST_F(FunctionInfoTest, BasicInfos) {
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Function foo = sourceFile.findFunction("foo");

  EXPECT_TRUE(foo.isValid());
  EXPECT_EQ("foo", foo.getName());
}

TEST_F(FunctionInfoTest, SearchForMissingFunction) {
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Function missing = sourceFile.findFunction("missing");

  EXPECT_FALSE(missing.isValid());
}

TEST_F(FunctionInfoTest, ReturnType) {
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Function foo = sourceFile.findFunction("foo");

  EXPECT_EQ("Int", foo.getResultType().getKindSpelling());
}

TEST_F(FunctionInfoTest, Parameters) {
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Function foo = sourceFile.findFunction("foo");

  EXPECT_EQ(2, foo.getParameters().size());
  EXPECT_EQ("f", foo.getParameters()[0].getName());
  EXPECT_EQ("c", foo.getParameters()[1].getName());
}

