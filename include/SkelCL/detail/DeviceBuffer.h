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
/// \file DeviceBuffer.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef DEVICE_BUFFER_H_
#define DEVICE_BUFFER_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include "Device.h"

namespace skelcl {

namespace detail {

class DeviceBuffer {
public:
  typedef size_t size_type;

  DeviceBuffer() = default;

  DeviceBuffer(const size_t deviceId,
               const size_t size,
               const size_t elemSize,
               cl_mem_flags flags = CL_MEM_READ_WRITE);

  DeviceBuffer(const DeviceBuffer& rhs);

  DeviceBuffer(DeviceBuffer&& rhs);

  DeviceBuffer& operator=(const DeviceBuffer&);

  DeviceBuffer& operator=(DeviceBuffer&& rhs);

  ~DeviceBuffer();

  std::shared_ptr<Device> devicePtr() const;

  size_type size() const;

  size_type elemSize() const;

  size_type sizeInBytes() const;

  const cl::Buffer& clBuffer() const;

  bool isValid() const;

  void markAsInUse() const;

  void markAsNotInUse() const;

private:
  std::string getInfo() const;

  std::shared_ptr<Device>         _device;
  size_type                       _size;
  size_type                       _elemSize;
  cl_mem_flags                    _flags; // TODO: Needed?
  cl::Buffer                      _buffer;
  mutable std::atomic<int>        _refCount;
  mutable std::condition_variable _isNotInUse;
  mutable std::mutex              _mutex;
};

} // namespace detail

} // namespace skelcl

#endif // DEVICE_BUFFER_H_

