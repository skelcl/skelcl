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
#include <ssedit/Typedef.h>

#include "Test.h"

class RenameTypedefTest : public ::testing::Test {
protected:
  RenameTypedefTest()
    : _testFileName("TestSource.cpp"),
      _input(R"(
typedef float TYPE;

TYPE foo(TYPE i, TYPE* fp) {
  return *fp + i;
}
)") {
    std::ofstream testFile(_testFileName, std::ios_base::trunc);
    testFile.write(_input.c_str(), _input.size());
  };

  ~RenameTypedefTest() {
    remove(_testFileName.c_str());
  };

  std::string _testFileName;
  std::string _input;
};

TEST_F(RenameTypedefTest, RenameTypedef) {
  // start test
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Typedef tdef = sourceFile.findTypedef("TYPE");
  sourceFile.commitReplaceType(tdef, "int");
  sourceFile.writeCommittedChanges();
  // end test

  std::ifstream modifiedFile(_testFileName);
  std::string output( (std::istreambuf_iterator<char>(modifiedFile)), (std::istreambuf_iterator<char>()) );

  std::string expected(
R"(
typedef int TYPE;

TYPE foo(TYPE i, TYPE* fp) {
  return *fp + i;
}
)");
  EXPECT_EQ(expected, output);
}

