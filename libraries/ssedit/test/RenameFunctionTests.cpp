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

#include <fstream>
#include <iterator>
#include <iostream>

#include <cstdio>

#include <ssedit/SourceFile.h>
#include <ssedit/Function.h>

#include "Test.h"

class RenameFunctionTest : public ::testing::Test {
protected:
  RenameFunctionTest()
    : _testFileName("TestSource.cpp"),
      _input(
R"(int foo(float f, const char& c) {
  return f + c;
};

void foo2() {
  char c = 4;
  foo(2.5f, c);
};

float foo3(int i, float* fp) {
  return *fp + i;
})") {
    std::ofstream testFile(_testFileName, std::ios_base::trunc);
    testFile.write(_input.c_str(), _input.size());
  };

  ~RenameFunctionTest() {
    remove(_testFileName.c_str());
  };

  std::string _testFileName;
  std::string _input;
};

TEST_F(RenameFunctionTest, RenameFunction) {
  // start test
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Function foo = sourceFile.findFunction("foo");
  sourceFile.commitRename(foo, "bar");
  sourceFile.writeCommittedChanges();
  // end test

  std::ifstream modifiedFile(_testFileName);
  std::string output( (std::istreambuf_iterator<char>(modifiedFile)), (std::istreambuf_iterator<char>()) );

  std::string expected(
R"(int bar(float f, const char& c) {
  return f + c;
};

void foo2() {
  char c = 4;
  bar(2.5f, c);
};

float foo3(int i, float* fp) {
  return *fp + i;
})");
  EXPECT_EQ(expected, output);
}

TEST_F(RenameFunctionTest, TryToRenameMissingFunction) {
  // start test
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Function func = sourceFile.findFunction("func");
  sourceFile.commitRename(func, "bar");
  sourceFile.writeCommittedChanges();
  // end test

  std::ifstream modifiedFile(_testFileName);
  std::string output( (std::istreambuf_iterator<char>(modifiedFile)), (std::istreambuf_iterator<char>()) );

  EXPECT_EQ(_input, output);
}

