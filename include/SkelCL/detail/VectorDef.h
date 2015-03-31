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
///  VectorDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef VECTOR_DEF_H_
#define VECTOR_DEF_H_

#include <algorithm>
#include <ios>
#include <iterator>
#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>

#include "../Distributions.h"
#include "../Source.h"

#include "Device.h"
#include "DeviceBuffer.h"
#include "DeviceList.h"
#include "Distribution.h"
#include "Event.h"
#include "Util.h"

namespace skelcl {

template <typename T>
RegisterVectorDeviceFunctions<T>::RegisterVectorDeviceFunctions()
{
  detail::CommonDefinitions::append(Vector<T>::deviceFunctions(),
      detail::CommonDefinitions::Level::GENERATED_DEFINITION);
}

template <typename T>
Vector<T>::Vector()
  : _size(0),
    _distribution(new detail::Distribution<Vector<T>>()),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(true),
    _hostBuffer(),
    _deviceBuffers(),
    _uevents(),
    _devents()
{
  (void)registerVectorDeviceFunctions;
  LOG_DEBUG_INFO("Created new Vector object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
Vector<T>::Vector(const size_type size,
                  const value_type& value,
                  const detail::Distribution<Vector<T>>& distribution)
  : _size(size),
    _distribution(detail::cloneAndConvert<Vector<T>>(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(size, value),
    _deviceBuffers(),
    _uevents(),
    _devents()
{
  (void)registerVectorDeviceFunctions;
  LOG_DEBUG_INFO("Created new Vector object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
template <typename InputIterator>
Vector<T>::Vector(InputIterator first, InputIterator last)
  : _size(std::distance(first, last)),
    _distribution(new detail::Distribution<Vector<T>>()),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(first, last),
    _deviceBuffers(),
    _uevents(),
    _devents()
{
  (void)registerVectorDeviceFunctions;
  LOG_DEBUG_INFO("Created new Vector object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
template <typename InputIterator>
Vector<T>::Vector(InputIterator first,
                  InputIterator last,
                  const detail::Distribution<Vector<T>>& distribution)
  : _size(std::distance(first, last)),
    _distribution(detail::cloneAndConvert<Vector<T>>(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(first, last),
    _deviceBuffers(),
    _uevents(),
    _devents()
{
  (void)registerVectorDeviceFunctions;
  LOG_DEBUG_INFO("Created new Vector object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
Vector<T>::Vector(const Vector<T>& rhs)
  : _size(rhs._size),
    _distribution(detail::cloneAndConvert<Vector<T>>(rhs.distribution())),
    _hostBufferUpToDate(rhs._hostBufferUpToDate),
    _deviceBuffersUpToDate(rhs._deviceBuffersUpToDate),
    _hostBuffer(rhs._hostBuffer),
    _deviceBuffers(rhs._deviceBuffers),
    _uevents(),
    _devents()
{
  (void)registerVectorDeviceFunctions;
  LOG_DEBUG_INFO("Created new Vector object (", this, ") by copying (", &rhs,
                 ") with ", getDebugInfo());
}

template <typename T>
Vector<T>::Vector(Vector<T>&& rhs)
  : _size(std::move(rhs._size)),
    _distribution(std::move(rhs._distribution)),
    _hostBufferUpToDate(std::move(rhs._hostBufferUpToDate)),
    _deviceBuffersUpToDate(std::move(rhs._deviceBuffersUpToDate)),
    _hostBuffer(std::move(rhs._hostBuffer)),
    _deviceBuffers(std::move(rhs._deviceBuffers)),
    _uevents(),
    _devents()
{
  (void)registerVectorDeviceFunctions;
  rhs._size = 0;
  rhs._hostBufferUpToDate = false;
  rhs._deviceBuffersUpToDate = false;
  LOG_DEBUG_INFO("Created new Vector object (", this, ") by moving from (",
                 &rhs,") with ", getDebugInfo());
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& rhs)
{
  if (this == &rhs) return *this; // handle self assignment
  _size                   = rhs._size;
  _distribution = detail::cloneAndConvert<Vector<T>>(rhs._distribution);
  _hostBufferUpToDate     = rhs._hostBufferUpToDate;
  _deviceBuffersUpToDate  = rhs._deviceBuffersUpToDate;
  _hostBuffer             = rhs._hostBuffer;
  _deviceBuffers          = rhs._deviceBuffers;
  LOG_DEBUG_INFO("Assignment to Vector object (", this, ") now with ",
                 getDebugInfo());
  return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& rhs)
{
  _size                   = std::move(rhs._size);
  _distribution           = std::move(rhs._distribution);
  _hostBufferUpToDate     = std::move(rhs._hostBufferUpToDate);
  _deviceBuffersUpToDate  = std::move(rhs._deviceBuffersUpToDate);
  _hostBuffer             = std::move(rhs._hostBuffer);
  _deviceBuffers          = std::move(rhs._deviceBuffers);
  rhs._size = 0;
  rhs._hostBufferUpToDate = false;
  rhs._deviceBuffersUpToDate = false;
  LOG_DEBUG_INFO("Move assignment to Vector object (", this, ") from (",
                 &rhs,") now with ", getDebugInfo());
  return *this;
}

template <typename T>
Vector<T>::~Vector()
{
  printEventTimings(_uevents, "ul");
  printEventTimings(_devents, "dl");
  LOG_DEBUG_INFO("Vector object (", this, ") with ", getDebugInfo(),
                 " destroyed");
}

template <typename T>
void Vector<T>::printEventTimings(std::vector<cl::Event> events,
                                  const std::string direction) const
{
  auto eventnum = 0;
  for (auto& e : events) {
    // Wait for job to complete.
    e.wait();

    // Get profiling information.
    auto start = e.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    auto end = e.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    auto runTime = (end - start) / static_cast<double>(1e6);

    // Print profiling information for event times, in the format:
    //
    //     Vector[<address>][<event>] <direction> <run> ms
    //
    // Where:
    //   <address>   is the memory address of the skeleton object;
    //   <event>     is an integer event number starting at 0, and
    //               incremented for each subsequent event;
    //   <direction> is either "ul" for upload or "dl" for download;
    //   <run>       "hw" execution time.
    LOG_PROF("Vector[", this, "][", eventnum, "] ",
             direction, " ", runTime, " ms");

    eventnum++; // Bump event counter.
  }
}

template <typename T>
typename Vector<T>::iterator Vector<T>::begin()
{
  copyDataToHost();
  return _hostBuffer.begin();
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::begin() const
{
  copyDataToHost();
  return _hostBuffer.begin();
}

template <typename T>
typename Vector<T>::iterator Vector<T>::end()
{
  copyDataToHost();
  return _hostBuffer.end();
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::end() const
{
  copyDataToHost();
  return _hostBuffer.end();
}

#if 0
template <typename T>
typename Vector<T>::reverse_iterator Vector<T>::rbegin()
{
  return _hostBuffer.rbegin();
}

template <typename T>
typename Vector<T>::const_reverse_iterator Vector<T>::rbegin() const
{
  return _hostBuffer.rbegin();
}

template <typename T>
typename Vector<T>::reverse_iterator Vector<T>::rend()
{
  return _hostBuffer.rend();
}

template <typename T>
typename Vector<T>::const_reverse_iterator Vector<T>::rend() const
{
  return _hostBuffer.rend();
}
#endif

template <typename T>
typename Vector<T>::size_type Vector<T>::size() const
{
  return _size;
}

template <typename T>
typename detail::Sizes Vector<T>::sizes() const
{
  ASSERT(_distribution != nullptr);

  detail::Sizes s;
  for (auto& devicePtr : _distribution->devices()) {
    s.push_back(this->_distribution->sizeForDevice(*this,
                                                  devicePtr->id()));
  }
  return s;
}

template <typename T>
typename Vector<T>::size_type Vector<T>::max_size() const
{
  // TODO: take device sizes into account
  return _hostBuffer.max_size();
}

template <typename T>
void Vector<T>::resize( typename Vector<T>::size_type sz, T c )
{
  _size = sz;
  if (_hostBufferUpToDate) {
    _hostBuffer.resize(sz, c);
    _deviceBuffersUpToDate = false;
    _deviceBuffers.clear();
  }
  LOG_DEBUG_INFO("Vector object (", this, ") resized, now with ",
                 getDebugInfo());
}

template <typename T>
typename Vector<T>::size_type Vector<T>::capacity() const
{
  return std::max(_hostBuffer.capacity(), _size);
}

template <typename T>
bool Vector<T>::empty() const
{
  return (_size == 0);
}

template <typename T>
void Vector<T>::reserve( typename Vector<T>::size_type n )
{
  return _hostBuffer.reserve(n);
}

template <typename T>
typename Vector<T>::reference Vector<T>::operator[]( typename Vector<T>::size_type n )
{
  copyDataToHost();
  return _hostBuffer.operator[](n);
}

template <typename T>
typename Vector<T>::const_reference
  Vector<T>::operator[]( typename Vector<T>::size_type n ) const
{
  copyDataToHost();
  return _hostBuffer.operator[](n);
}

template <typename T>
typename Vector<T>::reference Vector<T>::at( typename Vector<T>::size_type n )
{
  copyDataToHost();
  return _hostBuffer.at(n);
}

template <typename T>
typename Vector<T>::const_reference
  Vector<T>::at( typename Vector<T>::size_type n ) const
{
  copyDataToHost();
  return _hostBuffer.at(n);
}

template <typename T>
typename Vector<T>::reference Vector<T>::front()
{
  copyDataToHost();
  return _hostBuffer.front();
}

template <typename T>
typename Vector<T>::const_reference Vector<T>::front() const
{
  copyDataToHost();
  return _hostBuffer.front();
}

template <typename T>
typename Vector<T>::reference Vector<T>::back()
{
  copyDataToHost();
  return _hostBuffer.back();
}

template <typename T>
typename Vector<T>::const_reference Vector<T>::back() const
{
  copyDataToHost();
  return _hostBuffer.back();
}

template <typename T>
template <class InputIterator>
void Vector<T>::assign( InputIterator first, InputIterator last )
{
  _hostBuffer.assign(first, last);
}

template <typename T>
void Vector<T>::assign( typename Vector<T>::size_type n, const T& u )
{
  _hostBuffer.assign(n, u);
}

template <typename T>
void Vector<T>::push_back( const T& x )
{
  _hostBuffer.push_back(x);
  ++_size;
}

template <typename T>
void Vector<T>::pop_back()
{
  _hostBuffer.pop_back();
  --_size;
}

template <typename T>
typename Vector<T>::iterator Vector<T>::insert( typename Vector<T>::iterator position,
                                                const T& x )
{
  ++_size;
  return _hostBuffer.insert(position, x);
}

template <typename T>
void Vector<T>::insert( typename Vector<T>::iterator position,
                        typename Vector<T>::size_type n,
                        const T& x )
{
  _size += n;
  return _hostBuffer.insert(position, n, x);
}

template <typename T>
template <class InputIterator>
void Vector<T>::insert( typename Vector<T>::iterator position,
                        InputIterator first,
                        InputIterator last )
{
  _hostBuffer.insert(position, first, last);
  _size = _hostBuffer.size();
// TODO This is NOT Compiling !?!?: _size += std::distance(first, last);
}

template <typename T>
typename Vector<T>::iterator Vector<T>::erase( typename Vector<T>::iterator position )
{
  --_size;
  return _hostBuffer.erase(position);
}

template <typename T>
typename Vector<T>::iterator Vector<T>::erase( typename Vector<T>::iterator first,
                                               typename Vector<T>::iterator last )
{
  _size -= std::distance(first, last);
  return _hostBuffer.erase(first, last);
}

template <typename T>
void Vector<T>::swap( Vector<T>& rhs )
{
  // TODO: swap device buffers
  _hostBuffer.swap(rhs._hostBuffer);
  // swap sizes:
  size_type tmp = _size;
  _size = rhs._size;
  rhs._size = tmp;
}

template <typename T>
void Vector<T>::clear()
{
  _hostBuffer.clear();
  _size = 0;
}

template <typename T>
typename Vector<T>::allocator_type Vector<T>::get_allocator() const
{
  return _hostBuffer.get_allocator();
}

template <typename T>
detail::Distribution<Vector<T>>& Vector<T>::distribution() const
{
  ASSERT(_distribution != nullptr);
  return *_distribution;
}

template <typename T>
template <typename U>
void Vector<T>::setDistribution(const detail::Distribution<Vector<U>>&
                                    origDistribution) const
{
  ASSERT(origDistribution.isValid());
  // convert and set distribution
  this->setDistribution(detail::cloneAndConvert<Vector<T>>(origDistribution));
}

template <typename T>
template <typename U>
void Vector<T>::setDistribution(
        const std::unique_ptr<detail::Distribution<Vector<U>>>& origDistribution
                               ) const
{
  ASSERT(origDistribution != nullptr);
  ASSERT(origDistribution->isValid());
  // convert and set distribution
  this->setDistribution(detail::cloneAndConvert<Vector<T>>(*origDistribution));
}

template <typename T>
void Vector<T>::setDistribution(
        std::unique_ptr<detail::Distribution<Vector<T>>>&& newDistribution
                               ) const
{
  ASSERT(newDistribution != nullptr);
  ASSERT(newDistribution->isValid());

  if (   _distribution->isValid()
      && _distribution->dataExchangeOnDistributionChange(*newDistribution)) {
    copyDataToHost();
    _deviceBuffersUpToDate = false;
    _deviceBuffers.clear(); // delete old device buffers,
                            // so new can created using the new distribution
  }

  _distribution = std::move(newDistribution);
  ASSERT(_distribution->isValid());

  LOG_DEBUG_INFO("Vector object (", this,
                 ") assigned new distribution, now with ", getDebugInfo());
}

template <typename T>
void Vector<T>::createDeviceBuffers() const
{
  // create device buffers only if none have been created so far
  if (_deviceBuffers.empty()) {
    forceCreateDeviceBuffers();
  }
}

template <typename T>
void Vector<T>::forceCreateDeviceBuffers() const
{
  ASSERT(_size > 0);
  ASSERT(_distribution != nullptr);
  ASSERT(_distribution->isValid());

  _deviceBuffers.clear();

  std::transform( _distribution->devices().begin(),
                  _distribution->devices().end(),
                  std::inserter(_deviceBuffers, _deviceBuffers.end()),
        [this](std::shared_ptr<detail::Device> devicePtr) {
          return std::make_pair(
                    devicePtr->id(),
                    detail::DeviceBuffer(
                      devicePtr,
                      this->_distribution->sizeForDevice(
                              const_cast<Vector<T>&>(*this),
                              devicePtr ),
                      sizeof(T)
                      /*,mem flags*/ )
                 );
        } );
}

template <typename T>
detail::Event Vector<T>::startUpload() const
{
  ASSERT(_size > 0);
  ASSERT(_distribution != nullptr);
  ASSERT(_distribution->isValid());
  ASSERT(!_deviceBuffers.empty());

  detail::Event events;

  if (_deviceBuffersUpToDate) return events;

  _distribution->startUpload( const_cast<Vector<T>&>(*this), &events );

  _deviceBuffersUpToDate = true;

  LOG_DEBUG_INFO("Started data upload to ", _distribution->devices().size(),
           " devices (", getInfo(), ")");

  // Cache events to retrieve profiling information later.
  _uevents.insert(_uevents.end(), events.begin(), events.end());

  return events;
}

template <typename T>
void Vector<T>::copyDataToDevices() const
{
  if (_hostBufferUpToDate && !_deviceBuffersUpToDate) {
    startUpload().wait();
  }
}

template <typename T>
detail::Event Vector<T>::startDownload() const
{
  ASSERT(_size > 0);
  ASSERT(_distribution != nullptr);
  ASSERT(_distribution->isValid());
  ASSERT(!_deviceBuffers.empty());

  detail::Event events;

  if (_hostBufferUpToDate) return events;

  _hostBuffer.resize(_size); // make enough room to store data

  _distribution->startDownload( const_cast<Vector<T>&>(*this), &events );

  _hostBufferUpToDate = true;

  LOG_DEBUG_INFO("Started data download from ", _distribution->devices().size(),
                 " devices (", getInfo() ,")");

  // Cache events to retrieve profiling information later.
  _devents.insert(_devents.end(), events.begin(), events.end());

  return events;
}

template <typename T>
void Vector<T>::copyDataToHost() const
{
  if (_deviceBuffersUpToDate && !_hostBufferUpToDate) {
    startDownload().wait();
  }
}

template <typename T>
void Vector<T>::dataOnDeviceModified() const
{
  _hostBufferUpToDate     = false;
  _deviceBuffersUpToDate  = true;
  LOG_DEBUG_INFO("Data on devices marked as modified");
}

template <typename T>
void Vector<T>::dataOnHostModified() const
{
  _hostBufferUpToDate     = true;
  _deviceBuffersUpToDate  = false;
  LOG_DEBUG_INFO("Data on host marked as modified");
}

template <typename T>
const detail::DeviceBuffer&
  Vector<T>::deviceBuffer(const detail::Device& device) const
{
  return _deviceBuffers[device.id()];
}

template <typename T>
detail::DeviceBuffer& Vector<T>::deviceBuffer(const detail::Device& device)
{
  return _deviceBuffers[device.id()];
}

template <typename T>
typename Vector<T>::host_buffer_type& Vector<T>::hostBuffer() const
{
  return _hostBuffer;
}

template <typename T>
std::string Vector<T>::deviceFunctions()
{
  std::string type = detail::util::typeToString<T>();
  std::stringstream s;

  // found "double" => enable double
  if (type.find("double") != std::string::npos) {
    s << "#if defined(cl_khr_fp64)\n"
         "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n"
         "#elif defined(cl_amd_fp64)\n"
         "#pragma OPENCL EXTENSION cl_amd_fp64 : enable\n"
         "#endif\n";
  }

  return s.str();
}

template <typename T>
std::string Vector<T>::getInfo() const
{
  std::stringstream s;
  s << "size: "                   << _size;
#if 0
    << ", dist.: ";
       if (_distribution == nullptr)  s << "NULL";
  else if (_distribution->isSingle()) s << "single";
  else if (_distribution->isBlock())  s << "block";
  else if (_distribution->isCopy())   s << "copy";
#endif
  return s.str();
}

template <typename T>
std::string Vector<T>::getDebugInfo() const
{
  std::stringstream s;
  s << getInfo()
    << std::boolalpha
    << ", deviceBuffersCreated: "  << (!_deviceBuffers.empty())
    << ", hostBufferUpToDate: "    << _hostBufferUpToDate
    << ", deviceBuffersUpToDate: " << _deviceBuffersUpToDate
    << ", hostBuffer: "            << _hostBuffer.data();
  return s.str();
}

} // namespace skelcl

#endif // VECTOR_DEF_H_
