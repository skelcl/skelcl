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
#include <clang-c/Index.h>
// test utilities
#include "utilities.h"
// ssedit header
#include <ssedit/Type.h>
#include <ssedit/Cursor.h>

TEST(TypeTest, DefaultConstructor) {
  ssedit::Type type;

  EXPECT_FALSE(type.isValid());
  EXPECT_FALSE(type.isPointer());
  EXPECT_FALSE(type.isPOD());
  EXPECT_FALSE(type.isConstantQualified());
  EXPECT_FALSE(type.getPointeeType().isValid());
  EXPECT_FALSE(type.getResultType().isValid());
  EXPECT_FALSE(type.getCanonicalType().isValid());
  EXPECT_FALSE(type.getTypeDeclaration().isValid());
}

TEST(TypeTest, Kind) {
  TranslationUnit tu("int foo() {return 1;}");
  /* CXCursor cursor = */ clang_getTranslationUnitCursor(tu._tu);
}

TEST(TypeTest, Properties) {
}

TEST(TypeTest, PointeeType) {
}

TEST(TypeTest, ResultType) {
}

TEST(TypeTest, CannonicalType) {
}

TEST(TypeTest, TypeDeclaration) {
}

