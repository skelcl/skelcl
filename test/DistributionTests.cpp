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

#include <gtest/gtest.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Distribution.h>

class DistributionTest : public ::testing::Test {
protected:
  DistributionTest()
  {
    skelcl::init();
  }
  ~DistributionTest()
  {
    skelcl::terminate();
  }
};

TEST_F(DistributionTest, SingleDistribution)
{
  auto single = skelcl::Distribution::Single(0);

  EXPECT_TRUE(single->isSingle());
  EXPECT_FALSE(single->isBlock());
  EXPECT_FALSE(single->isCopy());
  EXPECT_EQ(1, single->devices().size());
  EXPECT_EQ(skelcl::detail::globalDeviceList[0].get(),
            single->device(0).get());
}

TEST_F(DistributionTest, BlockDistribution)
{
  auto block = skelcl::Distribution::Block();

  EXPECT_FALSE(block->isSingle());
  EXPECT_TRUE(block->isBlock());
  EXPECT_FALSE(block->isCopy());
  EXPECT_EQ(skelcl::detail::globalDeviceList.size(),
            block->devices().size());
  int i = 0;
  for (auto iter  = skelcl::detail::globalDeviceList.begin();
            iter != skelcl::detail::globalDeviceList.end();
          ++iter) {
    EXPECT_EQ( iter->get(), block->device(i++).get() );
  }
}

TEST_F(DistributionTest, CopyDistribution)
{
  auto copy = skelcl::Distribution::Copy();

  EXPECT_FALSE(copy->isSingle());
  EXPECT_FALSE(copy->isBlock());
  EXPECT_TRUE(copy->isCopy());
  EXPECT_EQ(skelcl::detail::globalDeviceList.size(),
            copy->devices().size());
  int i = 0;
  for (auto iter  = skelcl::detail::globalDeviceList.begin();
            iter != skelcl::detail::globalDeviceList.end();
          ++iter) {
    EXPECT_EQ( iter->get(), copy->device(i++).get() );
  }
}

