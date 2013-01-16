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
/// DeviceBuffer.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <sstream>
#include <string>
#include <memory>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>

#include "SkelCL/detail/DeviceBuffer.h"

#include "SkelCL/detail/Device.h"
#include "SkelCL/detail/DeviceList.h"

namespace {

using namespace skelcl::detail;

cl::Buffer createCLBuffer(const std::shared_ptr<Device>& devicePtr,
                          const size_t size,
                          const size_t elemSize,
                          cl_mem_flags flags) {
  cl::Buffer buffer;
  try {
    buffer = cl::Buffer(devicePtr->clContext(), flags, size * elemSize);
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }
  return buffer;
}

} // namespace

namespace skelcl {

namespace detail {

DeviceBuffer::DeviceBuffer()
  : _device(), _size(), _elemSize(), _flags(), _buffer()
{
}

DeviceBuffer::DeviceBuffer(const std::shared_ptr<Device>& devicePtr,
                           const size_t size,
                           const size_t elemSize,
                           cl_mem_flags flags)
  : _device(devicePtr),
    _size(size),
    _elemSize(elemSize),
    _flags(flags),
    _buffer(::createCLBuffer(_device, _size, _elemSize, _flags))
{
  LOG_DEBUG_INFO("Created new DeviceBuffer object (", this, ") with ",
                 getInfo());
}

DeviceBuffer::DeviceBuffer(const DeviceBuffer& rhs)
  : _device(rhs._device),
    _size(rhs._size),
    _elemSize(rhs._elemSize),
    _flags(rhs._flags),
    _buffer()
{
  // make deep copy of the rhs buffer
  _buffer = ::createCLBuffer(_device, _size, _elemSize, _flags);
  _device->enqueueCopy(rhs, *this);

  LOG_DEBUG_INFO("Created new DeviceBuffer object (", this, ") by copying (",
                 &rhs, ") with ", getInfo());
}

DeviceBuffer::DeviceBuffer(DeviceBuffer&& rhs)
  : _device(std::move(rhs._device)),
    _size(std::move(rhs._size)),
    _elemSize(std::move(rhs._elemSize)),
    _flags(std::move(rhs._flags)),
    _buffer(std::move(rhs._buffer)) // only wrapper object (pointer) is copied
{
  rhs._size     = 0;
  rhs._elemSize = 0;
  rhs._buffer   = cl::Buffer();
  LOG_DEBUG_INFO("Created new DeviceBuffer object (", this, ") by moving with ",
                 getInfo());
}

DeviceBuffer& DeviceBuffer::operator=(const DeviceBuffer& rhs)
{
  if (this == &rhs) return *this; // handle self assignement
  _device   = rhs._device;
  _size     = rhs._size;
  _elemSize = rhs._elemSize;
  _flags    = rhs._flags;
  // make deep copy of the rhs buffer
  _buffer   = ::createCLBuffer(_device, _size, _elemSize, _flags);
  _device->enqueueCopy(rhs, *this);

  LOG_DEBUG_INFO("Assignement to DeviceBuffer object (", this, ") now with ",
                 getInfo());
  return *this;
}

DeviceBuffer& DeviceBuffer::operator=(DeviceBuffer&& rhs)
{
  if (this == &rhs) return *this;
  _device   = std::move(rhs._device);
  _size     = std::move(rhs._size);
  _elemSize = std::move(rhs._elemSize);
  _flags    = std::move(rhs._flags);
  _buffer   = std::move(rhs._buffer); // copy only wrapper object (pointer)

  rhs._size     = 0;
  rhs._elemSize = 0;
  rhs._buffer   = cl::Buffer();
  LOG_DEBUG_INFO("Move assignment to DeviceBuffer object (", this, ") now with ",
                 getInfo());
  return *this;
}

DeviceBuffer::~DeviceBuffer()
{
  LOG_DEBUG_INFO("DeviceBuffer object (", this, ") destroyed");
  if (_buffer() != nullptr) {
    auto refCount = _buffer.getInfo<CL_MEM_REFERENCE_COUNT>();
    if (refCount > 1) {
      LOG_DEBUG_INFO("OpenCL Buffer object remains alive (Ref count ",
                     refCount, ")");
    }
  }
}

std::shared_ptr<Device> DeviceBuffer::devicePtr() const
{
  return _device;
}

DeviceBuffer::size_type DeviceBuffer::size() const
{
  return _size;
}

DeviceBuffer::size_type DeviceBuffer::elemSize() const
{
  return _elemSize;
}

DeviceBuffer::size_type DeviceBuffer::sizeInBytes() const
{
  return _size * _elemSize;
}

const cl::Buffer& DeviceBuffer::clBuffer() const
{
  return _buffer;
}

bool DeviceBuffer::isValid() const
{
  return (_buffer() != NULL);
}

std::string DeviceBuffer::getInfo() const
{
  std::stringstream s;
  s << "device: "   << _device->id()
    << ", size: "   << _size
    << ", flags: "  << _flags
    << ", buffer: " << _buffer();
  return s.str();
}

} // namespace detail

} // namespace skelcl

