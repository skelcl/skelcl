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

#include <SkelCL/detail/Device.h>

#include "Test.h"

class DeviceTest : public ::testing::Test {
protected:
  DeviceTest() : _platform(), _device() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    _platform = platforms[0];

    std::vector<cl::Device> devices;
    _platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    _device = devices[0];
  }

  ~DeviceTest() {
    // tear down
  }
  cl::Platform  _platform;
  cl::Device    _device;
};

TEST_F(DeviceTest, CreateDevice) {
  size_t id = 0;
  skelcl::detail::Device device(_device, _platform, id);

  EXPECT_EQ(id, device.id()); // test id
  EXPECT_EQ(_device(), device.clDevice()()); // test clDevice

  cl::Context context = device.clContext(); // test clContext
  cl_int err;
  std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>(&err);
  EXPECT_EQ(CL_SUCCESS, err); // getInfo succeeded
  EXPECT_EQ(1, devices.size()); // exactly one device found
  EXPECT_EQ(device.clDevice()(), devices[0]()); // device in context is the same device

  // TODO: Test command queue
}

