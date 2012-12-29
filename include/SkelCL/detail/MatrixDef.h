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
///  MatrixDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
/// \author Matthias Buss
///

#ifndef MATRIX_DEF_H_
#define MATRIX_DEF_H_

#include <algorithm>
#include <complex>
#include <ios>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "../Distributions.h"

#include "Assert.h"
#include "Device.h"
#include "DeviceBuffer.h"
#include "DeviceList.h"
#include "Event.h"
#include "Logger.h"

namespace skelcl {


template <typename T>
Matrix<T>::Matrix()
  : _size( {0,0} ),
    _distribution(new detail::Distribution<Matrix<T>>()),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(),
    _deviceBuffers()
{
  LOG_DEBUG_INFO("Created new Matrix object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
Matrix<T>::Matrix(const size_type size,
                  const value_type& value,
                  const detail::Distribution<Matrix<T>>& distribution)
  : _size(size),
    _distribution(detail::cloneAndConvert<T>(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer( _size.elemCount(), value ),
    _deviceBuffers()
{
  LOG_DEBUG_INFO("Created new Matrix object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
Matrix<T>::Matrix(const std::vector<T>& vector,
                  const size_type::size_type columnCount,
                  const detail::Distribution<Matrix<T>>& distribution)
  : _size({0,0}),
    _distribution(detail::cloneAndConvert<T>(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(vector),
    _deviceBuffers()
{
  auto rowCount = vector.size() / columnCount;
  if (vector.size() % columnCount == 0) {
    _size = { rowCount, columnCount };
  } else {
    _size = { rowCount + 1, columnCount };
    _hostBuffer.resize(_size.elemCount());
  }
  LOG_DEBUG_INFO("Created new Matrix object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
Matrix<T>::Matrix(const std::vector<T>& vector,
                  const size_type size,
                  const detail::Distribution<Matrix<T>>& distribution)
  : _size(size),
    _distribution(detail::cloneAndConvert<T>(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(vector),
    _deviceBuffers()
{
  _hostBuffer.resize(size.elemCount());
  LOG_DEBUG_INFO("Created new Matrix object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
Matrix<T>
  Matrix<T>::from2DVector(const std::vector<std::vector<T>>& input,
                          const detail::Distribution<Matrix<T>>& distribution)
{
  Matrix<T> matrix;
  matrix._size = {input.size(), input[0].size()};
  matrix._distribution = detail::cloneAndConvert<T>(distribution);
  matrix._hostBufferUpToDate = true;
  matrix._deviceBuffersUpToDate = false;
  matrix._hostBuffer.resize(matrix._size.elemCount());
  // start at the beginning
  auto iter = matrix._hostBuffer.begin();
  // for each row ...
  for (size_t row = 0; row < input.size(); ++row) {
    // ... copy the row into the host buffer ...
    std::copy(input[row].begin(), input[row].end(),
              iter);
    // ... advance pointer in host buffer
    std::advance(iter, matrix._size.columnCount());
  }
  LOG_DEBUG_INFO("Created new Matrix object (", &matrix, " with ",
                 matrix.getDebugInfo());
  return matrix;
}

template <typename T>
template <typename InputIterator>
Matrix<T>::Matrix(InputIterator first, InputIterator last,
                  const size_type::size_type columnCount,
                  const detail::Distribution<Matrix<T>>& distribution)
  : _size( {0,0} ),
    _distribution(detail::cloneAndConvert<T>(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(),
    _deviceBuffers()
{
  auto size = std::distance(first, last);
  auto rowCount = size / columnCount;
  if (size % columnCount == 0) {
    _size = { rowCount, columnCount };
  } else {
    _size = { rowCount + 1, columnCount };
  }
  _hostBuffer.assign(first, last);
  _hostBuffer.resize(_size.elemCount());
  LOG_DEBUG_INFO("Created new Matrix object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
template <typename InputIterator>
Matrix<T>::Matrix(InputIterator first, InputIterator last,
                  const size_type size,
                  const detail::Distribution<Matrix<T>>& distribution)
  : _size(size),
    _distribution(detail::cloneAndConvert<T>(distribution)),
    _hostBufferUpToDate(true),
    _deviceBuffersUpToDate(false),
    _hostBuffer(first, last),
    _deviceBuffers()
{
  _hostBuffer.resize(size.elemCount());
  LOG_DEBUG_INFO("Created new Matrix object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
Matrix<T>::Matrix(Matrix<T>&& rhs)
  : _size(std::move(rhs._size)),
    _distribution(std::move(rhs._distribution)),
    _hostBufferUpToDate(std::move(rhs._hostBufferUpToDate)),
    _deviceBuffersUpToDate(std::move(rhs._deviceBuffersUpToDate)),
    _hostBuffer(std::move(rhs._hostBuffer)),
    _deviceBuffers(std::move(rhs._deviceBuffers))
{
  _size = {0, 0};
  _hostBuffer.clear();

  LOG_DEBUG_INFO("Created new Matrix object (", this, ") with ",
                 getDebugInfo());
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(Matrix<T>&& rhs)
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
  LOG_DEBUG_INFO("Move assignment to Matrix object (", this, ") from (",
                  &rhs,") now with ", getDebugInfo());
  return *this;
}

template <typename T>
Matrix<T>::~Matrix()
{
  LOG_DEBUG_INFO("Matrix object (", this, ") with ", getDebugInfo(),
                 " destroyed");
}

template <typename T>
typename Matrix<T>::iterator Matrix<T>::begin()
{
  copyDataToHost();
  return _hostBuffer.begin();
}

template <typename T>
typename Matrix<T>::const_iterator Matrix<T>::begin() const
{
  copyDataToHost();
  return _hostBuffer.begin();
}

template <typename T>
typename Matrix<T>::iterator Matrix<T>::end()
{
  copyDataToHost();
  return _hostBuffer.end();
}

template <typename T>
typename Matrix<T>::const_iterator Matrix<T>::end() const
{
  copyDataToHost();
  return _hostBuffer.end();
}

template <typename T>
typename Matrix<T>::iterator
    Matrix<T>::row_begin(typename coordinate::index_type rowIndex)
{
  copyDataToHost();
  auto n = rowIndex * _size.columnCount();
  if ( n < _size.elemCount() ) {
    auto it  = _hostBuffer.begin();
    std::advance(it, n);
    return it;
  } else {
    return _hostBuffer.end();
  }
}

template <typename T>
typename Matrix<T>::const_iterator
    Matrix<T>::row_begin(typename coordinate::index_type rowIndex) const
{
  copyDataToHost();
  auto n = rowIndex * _size.columnCount();
  if ( n < _size.elemCount() ) {
    auto it = _hostBuffer.begin();
    std::advance(it, n);
    return it;
  } else {
    return _hostBuffer.end();
  }
}

template <typename T>
typename Matrix<T>::iterator
    Matrix<T>::row_end(typename coordinate::index_type rowIndex)
{
  return row_begin( rowIndex + 1 );
}

template <typename T>
typename Matrix<T>::const_iterator
    Matrix<T>::row_end(typename coordinate::index_type rowIndex) const
{
  return row_begin( rowIndex + 1 );
}

template <typename T>
typename Matrix<T>::iterator
    Matrix<T>::operator[]( typename coordinate::index_type rowIndex )
{
  return row_begin( rowIndex );
}

template <typename T>
typename Matrix<T>::const_iterator
    Matrix<T>::operator[]( typename coordinate::index_type rowIndex ) const
{
  return row_begin( rowIndex );
}

template <typename T>
typename Matrix<T>::size_type Matrix<T>::size() const
{
  return _size;
}

template <typename T>
typename Matrix<T>::size_type::size_type Matrix<T>::rowCount() const
{
  return _size.rowCount();
}

template <typename T>
typename Matrix<T>::size_type::size_type Matrix<T>::columnCount() const
{
  return _size.columnCount();
}

template <typename T>
typename Matrix<T>::size_type::size_type Matrix<T>::max_size() const
{
  return _hostBuffer.max_size();
}

template <typename T>
void Matrix<T>::resize(const size_type& size, T c)
{
  if (_hostBufferUpToDate) {
    _hostBuffer.resize(size.elemCount(), c);
    // device buffers are now invalid
    _deviceBuffers.clear();
    _deviceBuffersUpToDate = false;
  } else {
    // !_hostBufferUpToDate
    if (_deviceBuffersUpToDate) {
      // copy data down to the host
      copyDataToHost();
      // resize host buffer using the provided default value
      _hostBuffer.resize(size.elemCount(), c);
      // device buffers are now invalid
      _deviceBuffers.clear();
      _deviceBuffersUpToDate = false;
    }
  }

  // set size last, to avoid problems, when moving the "old" data around
  _size = size;

  LOG_DEBUG_INFO("Matrix object (", this, ") resized, now with ",
                 getDebugInfo());
}

template <typename T>
typename Matrix<T>::size_type::size_type Matrix<T>::capacity() const
{
  return _hostBuffer.capacity();
}

template <typename T>
bool Matrix<T>::empty() const
{
  return (   _size.rowCount()    == 0
          || _size.columnCount() == 0);
}

template <typename T>
void Matrix<T>::reserve(size_type::size_type bytes)
{
  // TODO: handling similar to resize ?
  return _hostBuffer.reserve(bytes);
}

template <typename T>
typename Matrix<T>::reference Matrix<T>::operator()( coordinate c )
{
  copyDataToHost();
  return _hostBuffer[c.rowIndex * _size.columnCount() + c.columnIndex];
}

template <typename T>
typename Matrix<T>::const_reference Matrix<T>::operator()( coordinate c ) const
{
  copyDataToHost();
  return _hostBuffer[c.rowIndex * _size.columnCount() + c.columnIndex];
}

template <typename T>
typename Matrix<T>::reference Matrix<T>::at( coordinate c )
{
  // check for out of bound access
  if (!(   c.rowIndex    < _size.rowCount()
        && c.columnIndex < _size.columnCount() )) {
    std::stringstream errSS;
    errSS << "Given index ("
          << c.rowIndex << ", " << c.columnIndex
          << ") for access is out of bound. "
          << "Boundaries are ("
          << _size.rowCount() << ", " << _size.columnCount()
          << ")";
    throw std::out_of_range(errSS.str());
  }

  copyDataToHost();
  return _hostBuffer[c.rowIndex * _size.columnCount() + c.columnIndex];
}

template <typename T>
typename Matrix<T>::const_reference Matrix<T>::at( coordinate c ) const
{
  // check for out of bound access
  if (!(   c.rowIndex    < _size.rowCount()
        && c.columnIndex < _size.columnCount() )) {
    std::stringstream errSS;
    errSS << "Given index ("
          << c.rowIndex << ", " << c.columnIndex
          << ") for access is out of bound. "
          << "Boundaries are ("
          << _size.rowCount() << ", " << _size.columnCount()
          << ")";
    throw std::out_of_range(errSS.str());
  }

  copyDataToHost();
  return _hostBuffer[c.rowIndex * _size.columnCount() + c.columnIndex];
}

template <typename T>
typename Matrix<T>::reference Matrix<T>::front()
{
  copyDataToHost();
  return _hostBuffer.front();
}

template <typename T>
typename Matrix<T>::const_reference Matrix<T>::front() const
{
  copyDataToHost();
  return _hostBuffer.front();
}

template <typename T>
typename Matrix<T>::reference Matrix<T>::back()
{
  copyDataToHost();
  return _hostBuffer.back();
}

template <typename T>
typename Matrix<T>::const_reference Matrix<T>::back() const
{
  copyDataToHost();
  return _hostBuffer.back();
}

template <typename T>
template <typename InputIterator>
void Matrix<T>::assign(InputIterator first, InputIterator last)
{
  copyDataToHost();

  _hostBuffer.assign(first, last);

  // calculate new number of rows
  auto dist = std::distance(first,last);

  auto columnCount = _size.columnCount();
  // if current column count == 0 => set to sqrt of range
  if (columnCount == 0) {
    columnCount = std::sqrt(dist);
  }

  auto rowCount = dist / columnCount;
  if (dist % columnCount != 0) {
    ++rowCount;
  }
  resize( {rowCount, columnCount} );

  // device buffers are not longer valid
  _deviceBuffersUpToDate = false;
  _deviceBuffers.clear();
}

template <typename T>
void Matrix<T>::assign(size_type size, const T& v )
{
  copyDataToHost();

  _hostBuffer.assign(size.rowCount() * size.columnCount(), v);
  _size = size;

  // device buffers are not longer valid
  _deviceBuffersUpToDate = false;
  _deviceBuffers.clear();
}

template <typename T>
template <class InputIterator>
void Matrix<T>::push_back_row(InputIterator first, InputIterator last)
{
  copyDataToHost();

  // range is bigger than column => cut range
  if (std::distance(first, last) > _size.columnCount()) {
    last = first + _size.columnCount();
  }

  // make room for one more row
  resize(_size.elemCount() + _size.columnCount());

  // get iterator to the end and ...
  auto insertPos = _hostBuffer.end();
  // ... jump back one row
  std::advance(insertPos, -_size.columnCount());

  // copy from range into _hostBuffer
  std::copy(first, last, insertPos);

  // set new size
  _size = { _size.rowCount()+1, _size.columnCount() };

  // device buffers are not longer valid
  _deviceBuffersUpToDate = false;
  _deviceBuffers.clear();
}

template <typename T>
void Matrix<T>::push_back_row(std::initializer_list<T> list)
{
  push_back_row(list.begin(), list.end());
}

template <typename T>
void Matrix<T>::pop_back_row()
{
  copyDataToHost();

  // resize container, which kicks off the last row
  resize( {_size.rowCount()-1, _size.columnCount()} );

  // device buffers are not longer valid
  _deviceBuffersUpToDate = false;
  _deviceBuffers.clear();
}

template <typename T>
template <class InputIterator>
void Matrix<T>::insert_row(typename coordinate::index_type rowIndex,
                           InputIterator first, InputIterator last)
{
  copyDataToHost();

  auto rangeSize = std::distance(first, last);

  // range is bigger than column => cut range
  if (rangeSize > _size.columnCount()) {
    last = first + _size.columnCount();
  }

  // reserve memory for one more row
  reserve( _size.elemCount() + _size.columnCount() );

  // get iterator at insert position
  auto insertPos = row_begin(rowIndex);

  // insert given range into host buffer
  _hostBuffer.insert(insertPos, first, last);

  // if range was smaller => fill row with default constructed elements
  if (rangeSize < _size.columnCount()) {
    // advance insert position
    std::advance(insertPos, rangeSize);
    // calculate how many elements are remaining
    auto remainingElements =  _size.columnCount() - rangeSize;
    // insert default constructed elements
    _hostBuffer.insert(insertPos, remainingElements, T());
  }

  // set new size
  _size = {_size.rowCount() + 1, _size.columnCount()};

  // device buffers are not longer valid
  _deviceBuffersUpToDate = false;
  _deviceBuffers.clear();
}

template <typename T>
void Matrix<T>::insert_row(typename coordinate::index_type rowIndex,
                           std::initializer_list<T> list)
{
  insert_row(rowIndex, list.begin(), list.end());
}

template <typename T>
typename Matrix<T>::iterator
  Matrix<T>::erase_row(typename coordinate::index_type rowIndex)
{
  _size = { _size.rowCount() -1 , _size.columnCount() };
  return _hostBuffer.erase(row_begin(rowIndex), row_end(rowIndex));
}

template <typename T>
void Matrix<T>::clear()
{
  _hostBuffer.clear();
  _deviceBuffers.clear();
  _size = {0,0};
}

template <typename T>
detail::Distribution<Matrix<T>>& Matrix<T>::distribution() const
{
  ASSERT(_distribution != nullptr);
  return *_distribution;
}

template <typename T>
template <typename U>
void Matrix<T>::setDistribution(const detail::Distribution<Matrix<U>>&
                                    origDistribution) const
{
  ASSERT(origDistribution.isValid());
  // convert and set distribution
  this->setDistribution(detail::cloneAndConvert<T>(origDistribution));
}

template <typename T>
template <typename U>
void Matrix<T>::setDistribution(
        const std::unique_ptr<detail::Distribution<Matrix<U>>>&
            origDistribution   ) const
{
  ASSERT(origDistribution != nullptr);
  ASSERT(origDistribution.isValid());
  // convert and set distribution
  this->setDistribution(detail::cloneAndConvert<T>(origDistribution));
}

template <typename T>
void Matrix<T>::setDistribution(
        std::unique_ptr<detail::Distribution<Matrix<T>>>&&
            newDistribution    ) const
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

  LOG_DEBUG_INFO("Matrix object (", this,
                 ") assigned new distribution, now with ", getDebugInfo());
}


template <typename T>
void Matrix<T>::createDeviceBuffers() const
{
  // create device buffers only if none have been created so far
  if (_deviceBuffers.empty()) {
    forceCreateDeviceBuffers();
  }
}

template <typename T>
void Matrix<T>::forceCreateDeviceBuffers() const
{
  ASSERT(_size.elemCount() > 0);
  ASSERT(_distribution != nullptr);

  _deviceBuffers.clear();

  std::transform( _distribution->devices().begin(),
                  _distribution->devices().end(),
                  std::inserter(_deviceBuffers, _deviceBuffers.begin()),
        [this](std::shared_ptr<detail::Device> devicePtr) {
                  return std::make_pair(
                            devicePtr->id(),
                            detail::DeviceBuffer(
                              devicePtr,
                              this->_distribution->sizeForDevice(
                                      const_cast<Matrix<T>&>(*this),
                                      devicePtr ),
                              sizeof(T)
                              /*,mem flags*/ )
                         );
        } );
}

template <typename T>
detail::Event Matrix<T>::startUpload() const
{
  ASSERT(_size.elemCount() > 0);
  ASSERT(_distribution != nullptr);
  ASSERT(_distribution->isValid());
  ASSERT(!_deviceBuffers.empty());

  detail::Event events;

  if (_deviceBuffersUpToDate) return events;

  _distribution->startUpload( const_cast<Matrix<T>&>(*this), &events );

  _deviceBuffersUpToDate = true;

  LOG_DEBUG_INFO("Started data upload to ", _distribution->devices().size(),
                 " devices (", getInfo(), ")");

  return events;
}

template <typename T>
void Matrix<T>::copyDataToDevices() const
{
  if (_hostBufferUpToDate && !_deviceBuffersUpToDate) {
    startUpload().wait();
  }
}

template <typename T>
detail::Event Matrix<T>::startDownload() const
{
  ASSERT(_size.elemCount() > 0);
  ASSERT(_distribution != nullptr);
  ASSERT(_distribution->isValid());
  ASSERT(!_deviceBuffers.empty());

  detail::Event events;

  if (_hostBufferUpToDate) return events;

  _hostBuffer.resize(_size.elemCount()); // make enough room to store data

  _distribution->startDownload( const_cast<Matrix<T>&>(*this), &events );

  _hostBufferUpToDate = true;

  LOG_DEBUG_INFO("Started data download from ", _distribution->devices().size(),
                 " devices (", getInfo() ,")");

  return events;
}

template <typename T>
void Matrix<T>::copyDataToHost() const
{
  if (_deviceBuffersUpToDate && !_hostBufferUpToDate) {
    startDownload().wait();
  }
}

template <typename T>
void Matrix<T>::dataOnDeviceModified() const
{
  _hostBufferUpToDate     = false;
  _deviceBuffersUpToDate  = true;
  LOG_DEBUG_INFO("Data on devices marked as modified");
}

template <typename T>
void Matrix<T>::dataOnHostModified() const
{
  _hostBufferUpToDate     = true;
  _deviceBuffersUpToDate  = false;
  LOG_DEBUG_INFO("Data on host marked as modified");
}

template <typename T>
const detail::DeviceBuffer&
  Matrix<T>::deviceBuffer(const detail::Device& device) const
{
  return _deviceBuffers[device.id()];
}

template <typename T>
typename Matrix<T>::host_buffer_type& Matrix<T>::hostBuffer() const
{
  return _hostBuffer;
}

template <typename T>
std::string Matrix<T>::deviceFunctions()
{
  return R"(
#define get(matrix, x, y) matrix[(int)(y * matrix##_col_count + x)]
#define set(matrix, x, y, value) matrix[(int)(y*matrix##_col_count+x)] = value
)";
}

template <typename T>
std::string Matrix<T>::getInfo() const
{
  std::stringstream s;
  s << "size: " << _size.elemCount();
  return s.str();
}

template <typename T>
std::string Matrix<T>::getDebugInfo() const
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

#endif // MATRIX_DEF_H_
