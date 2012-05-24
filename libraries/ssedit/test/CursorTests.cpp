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
// standard header
#include <fstream>
#include <string>
#include <cstdio>
// test utilities
#include "utilities.h"
// ssedit header
#include <ssedit/Cursor.h>
#include <ssedit/Type.h>

class CursorTest : public ::testing::Test
{
protected:
  CursorTest() : _tu("int main() { return 0; }")
  {
  };
  TranslationUnit _tu;
};

TEST_F(CursorTest, DefaultConstructor)
{
  ssedit::Cursor cursor;

  EXPECT_TRUE(cursor.isNullCursor());
  EXPECT_TRUE(cursor.isOfKind(CXCursor_FirstInvalid));
}

TEST_F(CursorTest, CXTranslationUnitConstructor)
{
  ssedit::Cursor cursor(_tu._tu);

  EXPECT_FALSE(cursor.isNullCursor());
  EXPECT_TRUE(cursor.isOfKind(CXCursor_TranslationUnit));
}

TEST_F(CursorTest, CopyConstructor)
{
  ssedit::Cursor cursor(_tu._tu);
  ssedit::Cursor copy(cursor);

  EXPECT_TRUE(clang_equalCursors(cursor.getCXCursor(),
                                 copy.getCXCursor()));
}

TEST_F(CursorTest, AssignementOperator)
{
  ssedit::Cursor cursor;
  ssedit::Cursor second(_tu._tu);
  cursor = second;

  EXPECT_FALSE(cursor.isNullCursor());
  EXPECT_TRUE(clang_equalCursors(cursor.getCXCursor(),
                                 second.getCXCursor()));
}

TEST_F(CursorTest, getType)
{
//  ssedit::Cursor nullCursor;
//  EXPECT_EQ(std::string(), nullCursor.getType().getKindSpelling());
}

TEST_F(CursorTest, setResultType)
{
}

TEST_F(CursorTest, Referenced)
{
}

TEST_F(CursorTest, getExtent)
{
}

TEST_F(CursorTest, getLocation)
{
}

TEST_F(CursorTest, findFunctionCursor)
{
}

TEST_F(CursorTest, findTypedefCursor)
{
}

TEST_F(CursorTest, gatherChildren)
{
}

TEST_F(CursorTest, Spelling)
{
}

TEST_F(CursorTest, Kind)
{
}

