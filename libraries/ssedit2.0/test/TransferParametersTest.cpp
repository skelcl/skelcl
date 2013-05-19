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

#include <string>

#include <ssedit2.0/Source.h>

#include "Test.h"

using namespace testing;

class TransferParametersTest : public Test
{
protected:
  TransferParametersTest() {}
};

TEST_F(TransferParametersTest, TransferSingleParameterToFunctionWithNoParameter)
{
  const char* input = "\
void bar\n\
(\n\
);\n\
void foo(int x, int y);\
";
  ssedit2::Source s(input);

  s.transferParameters("foo", 1, "bar");

  const char* expectedOutput = "\
void bar\n\
(\n\
int y);\n\
void foo(int x, int y);\
";
  ASSERT_EQ(expectedOutput, s.code());
}

TEST_F(TransferParametersTest, TransferSingleParameterToFunctionWithParameter)
{
  const char* input = "\
void bar(int i);\n\
void foo(int x, int y);\
";
  ssedit2::Source s(input);

  s.transferParameters("foo", 1, "bar");

  const char* expectedOutput = "\
void bar(int i, int y);\n\
void foo(int x, int y);\
";
  ASSERT_EQ(expectedOutput, s.code());
}

TEST_F(TransferParametersTest, TransferTwoParametersToFunctionWithParameter)
{
  const char* input = "\
void bar(int i);\n\
void foo(int x, int y, float z);\
";
  ssedit2::Source s(input);

  s.transferParameters("foo", 1, "bar");

  const char* expectedOutput = "\
void bar(int i, int y, float z);\n\
void foo(int x, int y, float z);\
";
  ASSERT_EQ(expectedOutput, s.code());
}

