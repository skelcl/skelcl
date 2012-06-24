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

#include "../Distributions.h"

#include "Assert.h"
#include "Device.h"
#include "DeviceBuffer.h"
#include "DeviceList.h"
#include "Distribution.h"
#include "Event.h"
#include "Logger.h"

namespace skelcl {

template <typename T>
Vector<T>::Vector()
  : _size(0),
    _distribution(new detail::Distribution< Vector<T> >()),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(true),
    _hostBuffer(),
    _deviceBuffers()
{
  LOG_DEBUG("Created new Vector object (", this, ") with ", getDebugInfo());
}

template <typename T>
Vector<T>::Vector(const size_type size, const value_type& value)
  : _size(size),
    _distribution(new detail::Distribution< Vector<T> >()),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(vector_type(size, value)),
    _deviceBuffers()
{
  LOG_DEBUG("Created new Vector object (", this, ") with ", getDebugInfo());
}

template <typename T>
Vector<T>::Vector(const size_type size,
                  const detail::Distribution< Vector<T> >& distribution,
                  const value_type& value)
  : _size(size),
    _distribution(new detail::Distribution< Vector<T> >(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(vector_type(size, value)),
    _deviceBuffers()
{
  ASSERT(distribution != nullptr);
  LOG_DEBUG("Created new Vector object (", this, ") with ", getDebugInfo());
}

template <typename T>
template <typename InputIterator>
Vector<T>::Vector(InputIterator first, InputIterator last)
  : _size(std::distance(first, last)),
    _distribution(new detail::Distribution< Vector<T> >()),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(first, last),
    _deviceBuffers()
{
  LOG_DEBUG("Created new Vector object (", this, ") with ", getDebugInfo());
}

template <typename T>
template <typename InputIterator>
Vector<T>::Vector(InputIterator first,
                  InputIterator last,
                  const detail::Distribution< Vector<T> >& distribution)
  : _size(std::distance(first, last)),
    _distribution(new detail::Distribution< Vector<T> >(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(first, last),
    _deviceBuffers()
{
  LOG_DEBUG("Created new Vector object (", this, ") with ", getDebugInfo());
}

template <typename T>
Vector<T>::Vector(const Vector<T>& rhs)
  : _size(rhs._size),
    _distribution(detail::cloneAndConvert<T>(rhs.distribution())),
    _hostBufferUpToDate(rhs._hostBufferUpToDate),
    _deviceBuffersUpToDate(rhs._deviceBuffersUpToDate),
    _hostBuffer(rhs._hostBuffer),
    _deviceBuffers(rhs._deviceBuffers)
{
  LOG_DEBUG("Created new Vector object (", this, ") by copying (", &rhs,
            ") with ", getDebugInfo());
}

template <typename T>
Vector<T>::Vector(Vector<T>&& rhs)
  : _size(rhs._size),
    _distribution(std::move(rhs._distribution)),
    _hostBufferUpToDate(rhs._hostBufferUpToDate),
    _deviceBuffersUpToDate(rhs._deviceBuffersUpToDate),
    _hostBuffer(std::move(rhs._hostBuffer)),
    _deviceBuffers(std::move(rhs._deviceBuffers))
{
  rhs._size = 0;
  rhs._hostBufferUpToDate = false;
  rhs._deviceBuffersUpToDate = false;
  LOG_DEBUG("Created new Vector object (", this, ") by moving from (",
    &rhs,") with ", getDebugInfo());
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& rhs)
{
  if (this == &rhs) return *this; // handle self assignment
  _size                   = rhs._size;
  _distribution           = detail::cloneAndConvert<T>(rhs._distribution);
  _hostBufferUpToDate     = rhs._hostBufferUpToDate;
  _deviceBuffersUpToDate  = rhs._deviceBuffersUpToDate;
  _hostBuffer             = rhs._hostBuffer;
  _deviceBuffers          = rhs._deviceBuffers;
  LOG_DEBUG("Assignment to Vector object (", this, ") now with ",
    getDebugInfo());
  return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& rhs)
{
  _size                   = rhs._size;
  _distribution           = std::move(rhs._distribution);
  _hostBufferUpToDate     = rhs._hostBufferUpToDate;
  _deviceBuffersUpToDate  = rhs._deviceBuffersUpToDate;
  _hostBuffer             = std::move(rhs._hostBuffer);
  _deviceBuffers          = std::move(rhs._deviceBuffers);
  rhs._size = 0;
  rhs._hostBufferUpToDate = false;
  rhs._deviceBuffersUpToDate = false;
  LOG_DEBUG("Move assignment to Vector object (", this, ") from (",
    &rhs,") now with ", getDebugInfo());
  return *this;
}

template <typename T>
Vector<T>::~Vector()
{
  LOG_DEBUG("Vector object (", this, ") with ", getDebugInfo(), " destroyed");
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
    s.push_back(this->_distribution->sizeForDevice(devicePtr->id(),
                                                   _size));
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
void Vector<T>::resize( Vector<T>::size_type sz, T c )
{
  _size = sz;
  if (_hostBufferUpToDate) {
    _hostBuffer.resize(sz, c);
    _deviceBuffersUpToDate = false;
    _deviceBuffers.clear();
  }
  LOG_DEBUG("Vector object (", this, ") resized, now with ", getDebugInfo());
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
void Vector<T>::reserve( Vector<T>::size_type n )
{
  return _hostBuffer.reserve(n);
}

template <typename T>
typename Vector<T>::reference Vector<T>::operator[]( Vector<T>::size_type n )
{
  copyDataToHost();
  return _hostBuffer.operator[](n);
}

template <typename T>
typename Vector<T>::const_reference
  Vector<T>::operator[]( Vector<T>::size_type n ) const
{
  copyDataToHost();
  return _hostBuffer.operator[](n);
}

template <typename T>
typename Vector<T>::reference Vector<T>::at( Vector<T>::size_type n )
{
  copyDataToHost();
  return _hostBuffer.at(n);
}

template <typename T>
typename Vector<T>::const_reference
  Vector<T>::at( Vector<T>::size_type n ) const
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
void Vector<T>::assign( Vector<T>::size_type n, const T& u )
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
typename Vector<T>::iterator Vector<T>::insert( Vector<T>::iterator position,
                                                const T& x )
{
  ++_size;
  return _hostBuffer.insert(position, x);
}

template <typename T>
void Vector<T>::insert( Vector<T>::iterator position,
                        Vector<T>::size_type n,
                        const T& x )
{
  _size += n;
  return _hostBuffer.insert(position, n, x);
}

template <typename T>
template <class InputIterator>
void Vector<T>::insert( Vector<T>::iterator position,
                        InputIterator first,
                        InputIterator last )
{
  _hostBuffer.insert(position, first, last);
  _size = _hostBuffer.size();
// TODO This is NOT Compiling !?!?: _size += std::distance(first, last);
}

template <typename T>
typename Vector<T>::iterator Vector<T>::erase( Vector<T>::iterator position )
{
  --_size;
  return _hostBuffer.erase(position);
}

template <typename T>
typename Vector<T>::iterator Vector<T>::erase( Vector<T>::iterator first,
                                               Vector<T>::iterator last )
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
detail::Distribution< Vector<T> >& Vector<T>::distribution() const
{
  ASSERT(_distribution != nullptr);
  return *_distribution;
}

template <typename T>
template <typename U>
void Vector<T>::setDistribution(const detail::Distribution< Vector<U> >& origDistribution) const
{
  ASSERT(origDistribution.isValid());

  // convert distribution to avoid problems later ...
  auto newDistribution = detail::cloneAndConvert<T>(origDistribution);

  // how to convert ???
  // auto newDistribution = origDistribution.clone();

  if (   _distribution->isValid()
      && _distribution->dataExchangeOnDistributionChange(*newDistribution)) {
    copyDataToHost();
    _deviceBuffers.clear(); // delete old device buffers,
                            // so new can created using the new distribution
  }

  _distribution = std::move(newDistribution);
  ASSERT(_distribution->isValid());

  LOG_DEBUG("Vector object (", this, ") assigned new distribution, now with ",
           getDebugInfo());
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

  _deviceBuffers.resize(_distribution->devices().size());
  std::transform( _distribution->devices().begin(),
                  _distribution->devices().end(),
                  _deviceBuffers.begin(),
        [this](std::shared_ptr<detail::Device> devicePtr) {
          return detail::DeviceBuffer(
                    devicePtr->id(),
                    this->_distribution->sizeForDevice(devicePtr->id(),
                                                       this->_size),
                    sizeof(T)
                    /*,mem flags*/ );
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

  LOG_INFO("Started data upload to ", _distribution->devices().size(),
           " devices (", getInfo(), ")");

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

  LOG_INFO("Started data download from ", _distribution->devices().size(),
           " devices (", getInfo() ,")");

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
  LOG_INFO("Data on devices marked as modified");
}

template <typename T>
void Vector<T>::dataOnHostModified() const
{
  _hostBufferUpToDate     = true;
  _deviceBuffersUpToDate  = false;
  LOG_INFO("Data on host marked as modified");
}

template <typename T>
const detail::DeviceBuffer&
  Vector<T>::deviceBuffer(const detail::Device& device) const
{
  return _deviceBuffers[device.id()];
}

template <typename T>
typename Vector<T>::vector_type& Vector<T>::hostBuffer() const
{
  return _hostBuffer;
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
    << ", hostBuffer: "            << &_hostBuffer.front();
  return s.str();
}

} // namespace skelcl

#endif // VECTOR_DEF_H_
