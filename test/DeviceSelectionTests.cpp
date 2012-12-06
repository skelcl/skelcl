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
#include <SkelCL/detail/Device.h>
#include <SkelCL/detail/DeviceProperties.h>
#include <SkelCL/detail/DeviceList.h>

class DeviceSelectionTest : public ::testing::Test {
protected:
  DeviceSelectionTest() {
    // skelcl::detail::defaultLogger.setLoggingLevel(skelcl::detail::Logger::Severity::Debug);
  };

  ~DeviceSelectionTest() {
    skelcl::terminate();
  };
};

using namespace skelcl;

TEST_F(DeviceSelectionTest, SelectAll) {
  skelcl::init(allDevices());
  EXPECT_GT(skelcl::detail::globalDeviceList.size(), 0);
}

TEST_F(DeviceSelectionTest, SelectAllGPUs) {
  skelcl::init( allDevices().deviceType(device_type::GPU) );
  EXPECT_GT(skelcl::detail::globalDeviceList.size(), 0);
  for (auto& device : skelcl::detail::globalDeviceList) {
    EXPECT_TRUE( device->isType(device_type::GPU) );
  }
}

TEST_F(DeviceSelectionTest, SelectOne) {
  skelcl::init(nDevices(1));
  EXPECT_EQ(skelcl::detail::globalDeviceList.size(), 1);
}

TEST_F(DeviceSelectionTest, SelectOneGPU) {
  skelcl::init(nDevices(1).deviceType(device_type::GPU));
  ASSERT_EQ(skelcl::detail::globalDeviceList.size(), 1);
  EXPECT_TRUE(skelcl::detail::globalDeviceList.front()->isType(skelcl::device_type::GPU));
}

TEST_F(DeviceSelectionTest, SelectFirstDeviceOfFirstPlatform)
{
  skelcl::init(skelcl::platform(0), skelcl::device(0));
  ASSERT_EQ(skelcl::detail::globalDeviceList.size(), 1);
}

