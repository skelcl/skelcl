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
/// \file DeviceProperties.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <string>
#include <limits>

#include "SkelCL/detail/DeviceProperties.h"

namespace {

} // namespace

namespace skelcl {

namespace detail {

DeviceProperties DeviceProperties::nDevices(size_t n)
{
  DeviceProperties dp;
  dp._count = n;
  return dp;
}

DeviceProperties DeviceProperties::allDevices()
{
  DeviceProperties dp;
  dp._takeAll = true;
  return dp;
}

DeviceProperties::DeviceProperties()
  : _deviceType(Device::Type::ALL),
    _takeAll(false),
    _count(0)//,
#if 0
    _id(std::numeric_limits<Device::id_type>::max()),
    _name(),
    _vendorName(),
    _maxClockFrequency(),
    _minComputeUnits(0),
    _maxWorkGroupSize(),
    _minWorkGroupSize(0),
    _maxWorkGroups(),
    _minWorkGroups(0),
    _globalMemSize(),
    _localMemSize(),
    _minGlobalMemSize(0),
    _minLocalMemSize(0)
#endif
{
}

DeviceProperties::~DeviceProperties()
{
}

bool DeviceProperties::match(const cl::Device& device) const
{
  if (    _deviceType != Device::Type::ALL
      &&  _deviceType != device.getInfo<CL_DEVICE_TYPE>()) return false;
  return true;
}

bool DeviceProperties::matchAndTake(const cl::Device& device)
{
  if (match(device)) {
    if (_takeAll) return true;
    // _takeAll == false
    if (_count > 0) {
      --_count; // take device out of set
      return true;
    }
  }
  return false;
}

DeviceProperties& DeviceProperties::deviceType(Device::Type value)
{
  _deviceType = value;
  return *this;
}

} // namespace detail

} // namespace skelcl
