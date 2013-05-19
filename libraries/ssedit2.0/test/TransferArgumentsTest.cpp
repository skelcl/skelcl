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

class TransferArgumentsTest : public Test
{
protected:
  TransferArgumentsTest() {}
};

TEST_F(TransferArgumentsTest, TransferSingleArgumentToFunctionWithOneArgument)
{
  const char* input = "\
int SCL_FUNC(...);\n\
int func(int x, int y) { return x + y; }\n\
\n\
void map(int* in, int* out, int elements) {\n\
  for (int i = 0; i < elements; ++i) {\n\
    out[i] = SCL_FUNC(in[i]);\n\
  }\n\
}\n\
";
  ssedit2::Source s(input);

  s.transferParameters("func", 1, "map");
  s.transferArguments("func", 1, "SCL_FUNC");

  const char* expectedOutput = "\
int SCL_FUNC(...);\n\
int func(int x, int y) { return x + y; }\n\
\n\
void map(int* in, int* out, int elements, int y) {\n\
  for (int i = 0; i < elements; ++i) {\n\
    out[i] = SCL_FUNC(in[i], y);\n\
  }\n\
}\n\
";
  ASSERT_EQ(expectedOutput, s.code());
}

TEST_F(TransferArgumentsTest, TransferSingleArgumentToFunctionWithNoArgument)
{
  const char* input = "\
int SCL_FUNC(...);\n\
int func(int x) { return -x; }\n\
\n\
void map(int* out, int elements) {\n\
  for (int i = 0; i < elements; ++i) {\n\
    out[i] = SCL_FUNC();\n\
  }\n\
}\n\
";
  ssedit2::Source s(input);

  s.transferParameters("func", 0, "map");
  s.transferArguments("func", 0, "SCL_FUNC");

  const char* expectedOutput = "\
int SCL_FUNC(...);\n\
int func(int x) { return -x; }\n\
\n\
void map(int* out, int elements, int x) {\n\
  for (int i = 0; i < elements; ++i) {\n\
    out[i] = SCL_FUNC(x);\n\
  }\n\
}\n\
";
  ASSERT_EQ(expectedOutput, s.code());
}

