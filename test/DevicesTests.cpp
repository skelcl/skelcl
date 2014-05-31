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

#include <SkelCL/SkelCL.h>
#include <SkelCL/detail/DeviceList.h>

#include "Test.h"
/// \cond
/// Don't show this test in doxygen

class DeviceListTest : public ::testing::Test {
protected:
  DeviceListTest() {
  }

  ~DeviceListTest() {
    // tear down
  }
};

TEST_F(DeviceListTest, Instance) {
  skelcl::detail::DeviceList devices;
  EXPECT_TRUE(devices.empty());
}

TEST_F(DeviceListTest, DeviceListInitOne) {
  skelcl::detail::DeviceList devices;
  devices.init(skelcl::nDevices(1));
  EXPECT_FALSE(devices.empty());
  EXPECT_EQ(1, devices.size());
}

TEST_F(DeviceListTest, DeviceListInit) {
  skelcl::detail::DeviceList devices;
  devices.init(skelcl::allDevices());
  EXPECT_FALSE(devices.empty());
  EXPECT_LT(0, devices.size()) << "Expected at least one device";
}

/// \endcond

