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
/// \file Device.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <functional>
#include <stdexcept>
#include <sstream>
#include <string>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>

#include "SkelCL/detail/Device.h"

#include "SkelCL/detail/DeviceBuffer.h"

namespace {

std::string printNDRange(const cl::NDRange& range)
{
  std::stringstream s;
  const size_t* sizes = range;
  s << "{ ";
  for (size_t i = 0; i < range.dimensions(); ++i) {
    s << sizes[i];
    if (i != range.dimensions()-1) {
      s << ", ";
    }
  }
  s << " }";
  return s.str();
}

void invokeCallback(cl_event /*event*/, cl_int status, void * userData)
{
  auto callback = static_cast<std::function<void()>*>(userData);
  (*callback)(); // invoke callback
  delete callback; // TODO: try to avoid explicit delete

  if (status != CL_COMPLETE) {
    LOG_ERROR("Event returned with abnormal status (", cl::Error(status), ")");
  }
}

} // namespace

namespace skelcl {

namespace detail {

Device::Device(const cl::Device& device,
               const cl::Platform& platform,
               const Device::id_type id)
  : _device(device), _context(), _commandQueue(), _id(id)
{
  try {
    VECTOR_CLASS<cl::Device> devices(1, _device);

    // create separate context for every device
    cl_context_properties props[] = {
                CL_CONTEXT_PLATFORM,
                reinterpret_cast<cl_context_properties>( (platform)() ),
                0
              };
    _context = cl::Context(devices, props);

    // create command queue for every device
    _commandQueue = cl::CommandQueue(_context, _device, CL_QUEUE_PROFILING_ENABLE);
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }

  LOG_INFO("Using device `", name(),
           "' with id: ", _id);
}

cl::Event Device::enqueue(const cl::Kernel& kernel,
                          const cl::NDRange& global,
                          const cl::NDRange& local,
                          const cl::NDRange& offset,
                          const std::function<void()> callback) const
{
  ASSERT(global.dimensions() == local.dimensions());
  ONLY_IN_DEBUG(
  auto globalSizeIsDivisiableByLocalSize = [&] () -> bool {
    bool isDivisiable = true;
    for (size_t i = 0; i < global.dimensions(); ++i) {
      if (global[i] % local[i] != 0) { isDivisiable = false; break; }
    }
    return isDivisiable;
  });
  ASSERT(globalSizeIsDivisiableByLocalSize());
  cl::Event event;
  try {
    _commandQueue.enqueueNDRangeKernel(kernel, offset, global, local,
                                       NULL, &event);
    _commandQueue.flush(); // always start calculation right away
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }

  // if callback is given, register the function to be called after the kernel
  // has finished
  if (callback != nullptr) {
    // copy function object to be used as user data
    // the pointer is deleted inside the invokeCallback wrapper function
    auto userData = static_cast<void*>(new std::function<void()>(callback));
    event.setCallback(CL_COMPLETE, ::invokeCallback, userData);
  }

  LOG_DEBUG_INFO("Kernel for device ", _id, " enqueued with global range: ",
                 ::printNDRange(global), ", local: ", ::printNDRange(local),
                 ", offset: ", ::printNDRange(offset));
  return event;
}

cl::Event Device::enqueueWrite(const  DeviceBuffer& buffer,
                               const void* hostPointer,
                               size_t hostOffset) const
{
  cl::Event event;
  try {
    _commandQueue.enqueueWriteBuffer(buffer.clBuffer(),
                                     CL_FALSE,
                                     0,
                                     buffer.sizeInBytes(),
                                     static_cast<const void*>(
                                       static_cast<const char*>(
                                         hostPointer)
                                       +(hostOffset * buffer.elemSize() ) ),
                                     NULL,
                                     &event);
    _commandQueue.flush(); // always start operation right away
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }

  LOG_DEBUG_INFO("Enqueued write buffer for device ", _id,
                 " (Bufferize: ", buffer.sizeInBytes(),
                 ", clBuffer: ", buffer.clBuffer()(),
                 ", deviceOffset: 0",
                 ", hostPointer: ", static_cast<const void*>(
                                       static_cast<const char*>(hostPointer)
                                     + (hostOffset * buffer.elemSize()) ),
                 ", hostOffset: ", hostOffset*buffer.elemSize() ,")");
  return event;
}

cl::Event Device::enqueueWrite(const  DeviceBuffer& buffer,
                               void* const hostPointer,
                               size_t size,
                               size_t deviceOffset,
                               size_t hostOffset) const
{
  cl::Event event;
  try {
    _commandQueue.enqueueWriteBuffer(buffer.clBuffer(),
                                     CL_FALSE,
                                     (deviceOffset * buffer.elemSize()),
                                     size * buffer.elemSize(),
                                     static_cast<void*const>(
                                       static_cast<char*const>(
                                         hostPointer)
                                       +(hostOffset * buffer.elemSize() ) ),
                                     NULL,
                                     &event);
    _commandQueue.flush(); // always start operation right away
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }

  LOG_DEBUG_INFO("Enqueued write buffer for device ", _id,
                 " (Buffersize: ", buffer.sizeInBytes(),
                 ", size: ", size,
                 ", clBuffer: ", buffer.clBuffer()(),
                 ", deviceOffset: ", deviceOffset*buffer.elemSize(),
                 ", hostPointer: ", static_cast<void*const>(
                                       static_cast<char*const>(hostPointer)
                                     + (hostOffset * buffer.elemSize()) ),
                 ", hostOffset: ", hostOffset*buffer.elemSize() ,")");
  return event;
}

cl::Event Device::enqueueRead(const DeviceBuffer& buffer,
                              void* hostPointer,
                              size_t hostOffset) const
{
  cl::Event event;
  try {
    _commandQueue.enqueueReadBuffer(buffer.clBuffer(),
                                    CL_FALSE,
                                    0,
                                    buffer.sizeInBytes(),
                                    static_cast<void*>(
                                      static_cast<char*>(
                                        hostPointer)
                                      +(hostOffset * buffer.elemSize()) ),
                                    NULL,
                                    &event);
    _commandQueue.flush(); // always start operation right away
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }

  LOG_DEBUG_INFO("Enqueued read buffer for device ", _id,
                 " (size: ", buffer.sizeInBytes(),
                 ", clBuffer: ", buffer.clBuffer()(),
                 ", deviceOffset: 0",
                 ", hostPointer: ", static_cast<void*>(
                                       static_cast<char*const>(hostPointer)
                                     + (hostOffset * buffer.elemSize()) ),
                 ", hostOffset: ", hostOffset * buffer.elemSize() ,")");
  return event;
}

cl::Event Device::enqueueRead(const DeviceBuffer& buffer,
                              void* const hostPointer,
                              size_t size,
                              size_t deviceOffset,
                              size_t hostOffset) const
{
  cl::Event event;
  try {
    _commandQueue.enqueueReadBuffer(buffer.clBuffer(),
                                    CL_FALSE,
                                    deviceOffset * buffer.elemSize(),
                                    size * buffer.elemSize(),
                                    static_cast<void*const>(
                                      static_cast<char*const>(
                                        hostPointer)
                                      +(hostOffset * buffer.elemSize()) ),
                                    NULL,
                                    &event);
    _commandQueue.flush(); // always start operation right away
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }

  LOG_DEBUG_INFO("Enqueued read buffer for device ", _id,
                 " (size: ", size * buffer.elemSize(),
                 ", clBuffer: ", buffer.clBuffer()(),
                 ", deviceOffset: ", deviceOffset * buffer.elemSize(),
                 ", hostPointer: ", static_cast<void*>(
                                       static_cast<char*const>(hostPointer)
                                     + (hostOffset * buffer.elemSize()) ),
                 ", hostOffset: ", hostOffset * buffer.elemSize() ,")");
  return event;
}

cl::Event Device::enqueueCopy(const DeviceBuffer& from,
                              const DeviceBuffer& to,
                              size_t fromOffset,
                              size_t toOffset) const
{
  ASSERT(    (from.sizeInBytes() - fromOffset)
          <= (to.sizeInBytes() - toOffset) );
  cl::Event event;
  try {
    _commandQueue.enqueueCopyBuffer(from.clBuffer(),
                                    to.clBuffer(),
                                    fromOffset,
                                    toOffset,
                                    from.sizeInBytes() - fromOffset,
                                    NULL,
                                    &event);
    _commandQueue.flush(); // always start operation right away
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }

  LOG_DEBUG_INFO("Enqueued copy buffer for device ", _id,
                 " (from: ", from.clBuffer()(),
                 ", to: ", to.clBuffer()(),
                 ", size: ", from.sizeInBytes() - fromOffset,
                 ", fromOffset: ", fromOffset,
                 ", toOffset: ", toOffset, ")");

  return event;
}

void Device::wait() const
{
  LOG_DEBUG_INFO("Start waiting for device with id: ", _id);
  try {
    _commandQueue.finish();
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }
  LOG_DEBUG_INFO("Finished waiting for device with id: ", _id);
}

Device::id_type Device::id() const
{
  return _id;
}

bool Device::isType(Type t) const
{
  return _device.getInfo<CL_DEVICE_TYPE>() == t;
}

std::string Device::name() const
{
  return _device.getInfo<CL_DEVICE_NAME>();
}

std::string Device::vendorName() const
{
  return _device.getInfo<CL_DEVICE_VENDOR>();
}

unsigned int Device::maxClockFrequency() const
{
  return _device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
}

unsigned int Device::maxComputeUnits() const
{
  return _device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
}

size_t Device::maxWorkGroupSize() const
{
  return _device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
}

size_t Device::maxWorkGroups() const
{
  return
    _device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[0] / maxWorkGroupSize();
}

unsigned long Device::globalMemSize() const
{
  return _device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
}

unsigned long Device::localMemSize() const
{
  return _device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
}

const cl::Context& Device::clContext() const
{
  return _context;
}

const cl::Device& Device::clDevice() const
{
  return _device;
}

bool Device::supportsDouble() const
{
  std::string extensions = _device.getInfo<CL_DEVICE_EXTENSIONS>();
  return (extensions.find("cl_khr_fp64") != std::string::npos);
}

std::istream& operator>>(std::istream& stream, Device::Type& type)
{
  std::string s;
  stream >> s;

       if (s == "ALL")         type = Device::Type::ALL;
  else if (s == "ANY")         type = Device::Type::ANY;
  else if (s == "CPU")         type = Device::Type::CPU;
  else if (s == "GPU")         type = Device::Type::GPU;
  else if (s == "ACCELERATOR") type = Device::Type::ACCELERATOR;
  else if (s == "DEFAULT")     type = Device::Type::DEFAULT;
  else throw std::invalid_argument(
    "Could not parse (" + s + ") as Device::Type.");

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const Device::Type& type)
{
  if (type == Device::Type::ANY)          return stream << "ANY";
  if (type == Device::Type::CPU)          return stream << "CPU";
  if (type == Device::Type::GPU)          return stream << "GPU";
  if (type == Device::Type::ACCELERATOR)  return stream << "ACCELERATOR";
  if (type == Device::Type::DEFAULT)      return stream << "DEFAULT";

  throw std::logic_error("This point should never be reached.");
}

} // namespace detail

} // namespace skelcl
