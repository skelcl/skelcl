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
  
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <string>

#include <pvsutil/Logger.h>

#include <SkelCL/detail/Util.h>

#include "Test.h"
/// \cond
/// Don't show this test in doxygen

class SHA1Test : public ::testing::Test {
protected:
  SHA1Test() {
    pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Debug);
  }

  ~SHA1Test() {
  }
};

TEST_F(SHA1Test, EmptyHash) {
  std::string text("");
  std::string expected("da39a3ee5e6b4b0d3255bfef95601890afd80709");

  EXPECT_EQ(expected, skelcl::detail::util::hash(text));
}

TEST_F(SHA1Test, lazyDog) {
  std::string text("The quick brown fox jumps over the lazy dog");
  std::string expected("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");

  EXPECT_EQ(expected, skelcl::detail::util::hash(text));
}

TEST_F(SHA1Test, lazyCog) {
  std::string text("The quick brown fox jumps over the lazy cog");
  std::string expected("de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3");

  EXPECT_EQ(expected, skelcl::detail::util::hash(text));
}

/// \endcond

