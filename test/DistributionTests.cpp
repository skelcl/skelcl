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
#include <SkelCL/Distributions.h>
#include <SkelCL/Vector.h>

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

TEST_F(DistributionTest, DefaultDistribution)
{
  auto dist = skelcl::detail::Distribution< skelcl::Vector<int> >();

  EXPECT_FALSE(dist.isValid());
}

TEST_F(DistributionTest, DefaultDistribution2)
{
  skelcl::Vector<int> vi;
  auto dist = skelcl::distribution::Default(vi);

  EXPECT_TRUE(dist != nullptr);
  EXPECT_FALSE(dist->isValid());
}

TEST_F(DistributionTest, SingleDistribution)
{
  auto single = skelcl::detail::SingleDistribution< skelcl::Vector<int> >();

  EXPECT_TRUE(single.isValid());
  EXPECT_EQ(1, single.devices().size());
  EXPECT_EQ(skelcl::detail::globalDeviceList[0].get(),
            single.device(0).get());
}

TEST_F(DistributionTest, SingleDistribution2)
{
  skelcl::Vector<int> vi;
  auto single = skelcl::distribution::Single(vi);

  EXPECT_TRUE(single != nullptr);
  EXPECT_TRUE(single->isValid());
  EXPECT_EQ(1, single->devices().size());
  EXPECT_EQ(skelcl::detail::globalDeviceList[0].get(),
            single->device(0).get());
}

TEST_F(DistributionTest, BlockDistribution)
{
  auto block = skelcl::detail::BlockDistribution< skelcl::Vector<int> >();

  EXPECT_TRUE(block.isValid());
  EXPECT_EQ(skelcl::detail::globalDeviceList.size(),
            block.devices().size());
  int i = 0;
  for (auto iter  = skelcl::detail::globalDeviceList.begin();
            iter != skelcl::detail::globalDeviceList.end();
          ++iter) {
    EXPECT_EQ( iter->get(), block.device(i++).get() );
  }
}

TEST_F(DistributionTest, BlockDistribution2)
{
  skelcl::Vector<int> vi;
  auto block = skelcl::distribution::Block(vi);

  EXPECT_TRUE(block != nullptr);
  EXPECT_TRUE(block->isValid());
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
  auto copy = skelcl::detail::CopyDistribution< skelcl::Vector<int> >();

  EXPECT_TRUE(copy.isValid());
  EXPECT_EQ(skelcl::detail::globalDeviceList.size(),
            copy.devices().size());
  int i = 0;
  for (auto iter  = skelcl::detail::globalDeviceList.begin();
            iter != skelcl::detail::globalDeviceList.end();
          ++iter) {
    EXPECT_EQ( iter->get(), copy.device(i++).get() );
  }
}

TEST_F(DistributionTest, CopyDistribution2)
{
  skelcl::Vector<int> vi;
  auto copy = skelcl::distribution::Copy(vi);

  EXPECT_TRUE(copy != nullptr);
  EXPECT_TRUE(copy->isValid());
  EXPECT_EQ(skelcl::detail::globalDeviceList.size(),
            copy->devices().size());
  int i = 0;
  for (auto iter  = skelcl::detail::globalDeviceList.begin();
            iter != skelcl::detail::globalDeviceList.end();
          ++iter) {
    EXPECT_EQ( iter->get(), copy->device(i++).get() );
  }
}

