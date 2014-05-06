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
/// \file DeviceProperties.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef DEVICE_PROPERTIES_H_
#define DEVICE_PROPERTIES_H_

#include <string>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include "Device.h"
#include "skelclDll.h"

namespace skelcl {

namespace detail {

class SKELCL_DLL DeviceProperties {
public:
  static DeviceProperties nDevices(size_t n);

  static DeviceProperties allDevices();

  virtual ~DeviceProperties();

  bool match(const cl::Device& device) const;

  bool matchAndTake(const cl::Device& device);

  DeviceProperties& deviceType(Device::Type value);
#if 0
  void id(Device::id_type value);
  void name(std::string value);
  void vendorName(std::string value);
  void maxClockFrequency(unsigned int value);
  void minClockFrequency(unsigned int value);
  void maxComputeUnits(unsigned int value);
  void minComputeUnits(unsigned int value);
  void maxWorkGroupSize(size_t value);
  void minWorkGroupSize(size_t value);
  void maxWorkGroups(size_t value);
  void minWorkGroups(size_t value);
  void globalMemSize(unsigned long value);
  void localMemSize(unsigned long value);
  void minGlobalMemSize(unsigned long value);
  void minLocalMemSize(unsigned long value);
#endif

private:
  DeviceProperties();

  Device::Type    _deviceType;
  bool            _takeAll;
  size_t          _count;
#if 0
  Device::id_type _id;
  std::string     _name;
  std::string     _vendorName;
  unsigned int    _maxClockFrequency;
  unsigned int    _minClockFrequency;
  unsigned int    _maxComputeUnits;
  unsigned int    _minComputeUnits;
  size_t          _maxWorkGroupSize;
  size_t          _minWorkGroupSize;
  size_t          _maxWorkGroups;
  size_t          _minWorkGroups;
  unsigned long   _globalMemSize;
  unsigned long   _localMemSize;
  unsigned long   _minGlobalMemSize;
  unsigned long   _minLocalMemSize;
#endif
};

} // namespace detail

} // namespace skelcl

#endif // DEVICE_PROPERTIES_H_

