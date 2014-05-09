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
#include <SkelCL/detail/Device.h>
#include <SkelCL/detail/DeviceProperties.h>
#include <SkelCL/detail/DeviceList.h>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include "Test.h"

class DeviceSelectionTest : public ::testing::Test {
protected:
  DeviceSelectionTest() : cpuCount(0), gpuCount(0) {
    //skelcl::detail::defaultLogger.setLoggingLevel(
    //    skelcl::detail::Logger::Severity::Debug);

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.size() > 0) {
      // for each platform ...
      for (auto& platform : platforms) {
        // .. get all devices ..
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        for (auto& device : devices) {
          auto type = device.getInfo<CL_DEVICE_TYPE>();
          if (type == CL_DEVICE_TYPE_CPU) {
            cpuCount++;
          } else if (type == CL_DEVICE_TYPE_GPU) {
            gpuCount++;
          }
        }
      }
    }
  }

  ~DeviceSelectionTest() {
    skelcl::terminate();
  }

  int cpuCount;
  int gpuCount;
};

using namespace skelcl;

TEST_F(DeviceSelectionTest, SelectAll) {
  if (cpuCount + gpuCount > 0) {
    skelcl::init(allDevices());
    EXPECT_GE(skelcl::detail::globalDeviceList.size(), cpuCount + gpuCount);
  }
}

TEST_F(DeviceSelectionTest, SelectAllGPUs) {
  if (cpuCount + gpuCount > 0) {
    skelcl::init( allDevices().deviceType(device_type::GPU) );
    EXPECT_EQ(skelcl::detail::globalDeviceList.size(), gpuCount);
    for (auto& device : skelcl::detail::globalDeviceList) {
      EXPECT_TRUE( device->isType(device_type::GPU) );
    }
  }
}

TEST_F(DeviceSelectionTest, SelectOne) {
  if (cpuCount + gpuCount > 0) {
    skelcl::init(nDevices(1));
    EXPECT_EQ(skelcl::detail::globalDeviceList.size(), 1);
  }
}

TEST_F(DeviceSelectionTest, SelectOneGPU) {
  if (cpuCount + gpuCount > 0) {
    skelcl::init(nDevices(1).deviceType(device_type::GPU));
    if (gpuCount > 0) {
      ASSERT_EQ(skelcl::detail::globalDeviceList.size(), 1);
      EXPECT_TRUE(skelcl::detail::globalDeviceList.front()->isType(skelcl::device_type::GPU));
    }
  }
}

TEST_F(DeviceSelectionTest, SelectFirstDeviceOfFirstPlatform)
{
  if (cpuCount + gpuCount > 0) {
    skelcl::init(skelcl::platform(0), skelcl::device(0));
    ASSERT_EQ(skelcl::detail::globalDeviceList.size(), 1);
  }
}

