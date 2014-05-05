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
  stooling::SourceCode s(input);

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
  stooling::SourceCode s(input);

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
  stooling::SourceCode s(input);

  s.transferParameters("foo", 1, "bar");

  const char* expectedOutput = "\
void bar(int i, int y, float z);\n\
void foo(int x, int y, float z);\
";
  ASSERT_EQ(expectedOutput, s.code());
}

TEST_F(TransferParametersTest, TransferTwoParametersToFunctionWithParameterInOpenCL)
{
  const char* input = "\
void foo(int x, int y, float z);\n\
__kernel void bar(__global int* i);\
";
  stooling::SourceCode s(input);

  s.transferParameters("foo", 1, "bar");

  const char* expectedOutput = "\
void foo(int x, int y, float z);\n\
__kernel void bar(__global int* i, int y, float z);\
";
  ASSERT_EQ(expectedOutput, s.code());
}


TEST_F(TransferParametersTest, ZipTest)
{
const char* input = R"(
#ifndef float_MATRIX_T
typedef struct {
  __global float* data;
  unsigned int col_count;
} float_matrix_t;
#define float_MATRIX_T
#endif

#ifndef MATRIX_GET
#define get(m, y, x) m.data[(int)((y) * m.col_count + (x))]
#define MATRIX_GET
#endif
#ifndef MATRIX_SET
#define set(m, y, x, v) m.data[(int)((y) * m.col_count + (x))] = (v)
#define MATRIX_SET
#endif
float func(float x, float y){ return x*y; }

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;
typedef float SCL_TYPE_2;

__kernel void SCL_ZIP(
    const __global SCL_TYPE_0*  SCL_LEFT,
    const __global SCL_TYPE_1*  SCL_RIGHT,
          __global SCL_TYPE_2*  SCL_OUT,
    const unsigned int          SCL_ELEMENTS ) {
  if (get_global_id(0) < SCL_ELEMENTS) {
    SCL_OUT[get_global_id(0)] = SCL_FUNC(SCL_LEFT[get_global_id(0)],
                                         SCL_RIGHT[get_global_id(0)]);
  }
})";
  
  stooling::SourceCode s(input);

  s.transferParameters("func", 2, "SCL_ZIP");

  const char* expectedOutput = R"(
#ifndef float_MATRIX_T
typedef struct {
  __global float* data;
  unsigned int col_count;
} float_matrix_t;
#define float_MATRIX_T
#endif

#ifndef MATRIX_GET
#define get(m, y, x) m.data[(int)((y) * m.col_count + (x))]
#define MATRIX_GET
#endif
#ifndef MATRIX_SET
#define set(m, y, x, v) m.data[(int)((y) * m.col_count + (x))] = (v)
#define MATRIX_SET
#endif
float func(float x, float y){ return x*y; }

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;
typedef float SCL_TYPE_2;

__kernel void SCL_ZIP(
    const __global SCL_TYPE_0*  SCL_LEFT,
    const __global SCL_TYPE_1*  SCL_RIGHT,
          __global SCL_TYPE_2*  SCL_OUT,
    const unsigned int          SCL_ELEMENTS ) {
  if (get_global_id(0) < SCL_ELEMENTS) {
    SCL_OUT[get_global_id(0)] = SCL_FUNC(SCL_LEFT[get_global_id(0)],
                                         SCL_RIGHT[get_global_id(0)]);
  }
})";
  ASSERT_EQ(expectedOutput, s.code());
}

