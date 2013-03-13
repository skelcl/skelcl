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
#include <ssedit/Type.h>
#include <ssedit/Typedef.h>

#include "Test.h"

class TypedefInfoTest : public ::testing::Test {
protected:
  TypedefInfoTest()
    : _testFileName("TestSource.cpp"),
      _input(R"(
typedef float TYPE;

int foo(TYPE f, const char& c) {
  return f + c;
})") {
    std::ofstream testFile(_testFileName, std::ios_base::trunc);
    testFile.write(_input.c_str(), _input.size());
  };

  ~TypedefInfoTest() {
    remove(_testFileName.c_str());
  };

  std::string _testFileName;
  std::string _input;
};

TEST_F(TypedefInfoTest, BasicInfos) {
  ssedit::SourceFile sourceFile(_testFileName);
  ssedit::Typedef tdef = sourceFile.findTypedef("TYPE");

  EXPECT_TRUE(tdef.isValid());
  EXPECT_EQ("TYPE", tdef.getName());
}

