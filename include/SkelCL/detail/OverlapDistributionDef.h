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
/// \file OverlapDistributionDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
/// \author Mathias Buss
///

#include "Padding.h"

namespace skelcl {

namespace detail {

template <typename T>
OverlapDistribution< Vector<T> >::OverlapDistribution(
      Vector<T>::size_type overlapRadius,
      Padding padding,
      T neutralElement,
      const DeviceList& deviceList)
  : Distribution< Vector<T> >(deviceList),
    _overlapRadius(overlapRadius),
    _padding(padding),
    _neutralElement(neutralElement)
{
}

template <typename T>
OverlapDistribution< Matrix<T> >::OverlapDistribution(
      Matrix<T>::size_type::size_type overlapRadius,
      Padding padding,
      T neutralElement,
      const DeviceList& deviceList)
  : Distribution< Matrix<T> >(deviceList),
    _overlapRadius(overlapRadius),
    _padding(padding),
    _neutralElement(neutralElement)
{
}

template <typename T>
template <typename U>
OverlapDistribution< Vector<T> >::OverlapDistribution( const OverlapDistribution< Vector<U> >& rhs)
  : Distribution< Vector<T> >(rhs),
    _overlapRadius(rhs.overlapRadius()),
    _padding(rhs.padding()),
    _neutralElement(rhs.neutralElement())
{
}

template <typename T>
template <typename U>
OverlapDistribution< Matrix<T> >::OverlapDistribution( const OverlapDistribution< Matrix<U> >& rhs)
  : Distribution< Matrix<T> >(rhs),
    _overlapRadius(rhs.overlapRadius()),
    _padding(rhs.padding()),
    _neutralElement(rhs.neutralElement())
{
}

template <typename T>
OverlapDistribution< Vector<T> >::~OverlapDistribution()
{
}

template <typename T>
OverlapDistribution< Matrix<T> >::~OverlapDistribution()
{
}

template <typename T>
bool OverlapDistribution< Vector<T> >::isValid() const
{
  return true;
}

template <typename T>
bool OverlapDistribution< Matrix<T> >::isValid() const
{
  return true;
}

template <typename T>
void OverlapDistribution< Vector<T> >::startUpload(Vector<T>& vector,
                                                   Event* events) const
{
  ASSERT(events != nullptr);
  std::vector<T> paddingFront;
  std::vector<T> paddingBack;

  switch (_padding)
  {
    case Padding::NEAREST:
      paddingFront.resize(_overlapRadius, vector.front());
      paddingBack.resize(_overlapRadius, vector.back());
      break;
    case Padding::NEUTRAL:
      paddingFront.resize(_overlapRadius, _neutralElement);
      paddingBack.resize(_overlapRadius, _neutralElement);
      break;
  }

  // Upload front paddint to first device
  auto& firstDevicePtr = _devices.front();
  auto event = firstDevicePtr->enqueueWrite(
                 vector.deviceBuffer(firstDevicePtr),
                 paddingFront.begin(),
                 paddingFront.size(),
                 0 /* deviceOffset */);
  events->insert(event);

  size_t offset       = 0;
  size_t deviceOffset = paddingFront.size();
  size_t devSize      = _devices.size();

  for (size_t i = 0; i < devSize; ++i) {
    auto& devicePtr = _devices[i];
    auto& buffer    = vector.deviceBuffer(devicePtr);

    auto size = buffer.size();

    if (i == 0)         size -= paddingFront.size();
    if (i == devSize-1) size -= paddingBack.size();

    event = devicePtr->enqueueWrite(buffer,
                                    vector.hostBuffer().begin(),
                                    size,
                                    deviceOffset,
                                    hostOffset);
    events->insert(event);

    hostOffset += size - _overlapRadius;
    deviceOffset = 0; // after the first device, the device offset is 0
  }

  // upload back padding at the end of last device
  auto& lastDevicePtr = _devices.back();
  // calculate offset on the device ...
  deviceOffset = vector.deviceBuffer(lastDevicePtr).size() - paddingBack.size();

  event = lastDevicePtr->enqueueWrite(
            vector.deviceBuffer(lastDevicePtr),
            paddingBack.begin(),
            paddingBack.size(),
            deviceOffset );
  events->insert(event);
}

template <typename T>
void OverlapDistribution< Matrix<T> >::startUpload(Matrix<T>& matrix,
                                                   Event* events) const
{
  ASSERT(events != nullptr);

  // some shortcuts ...
  auto columnCount   = matrix.size().columnCount();

  // create vectors for additional rows
  // initialize with neutral value for SCL_NEUTRAL
  // (override this in differnt case later)
  auto newSize = _overlapRadius * columnCount;
  Matrix<T>::host_buffer_type paddingTop(newSize, _neutralElement);
  Matrix<T>::host_buffer_type paddingBottom(newSize, _neutralElement);

  if (_padding == Padding::SCL_NEAREST) {
    // start at the beginning ...
    auto topInsert    = paddingTop.begin();
    auto bottomInsert = paddingBottom.begin();

    for (size_t i = 0; i < _overlapRadius; ++i) {
      // ... copy first row into padding vector ...
      std::copy(matrix.row_begin(0), matrix.row_end(0), topInsert);
      // ... and advance insert position
      std::advance(topInsert, columnCount);

      std::copy(matrix.row_begin(0), matrix.row_end(0), bottomInsert);
      std::advance(bottomInsert, columnCount);
    }
  }

  // Upload top padding to first device
  auto& firstDevicePtr = _devices.front();
  auto event = firstDevicePtr->enqueueWrite(
                 matrix.deviceBuffer(firstDevicePtr),
                 paddingTop.begin(),
                 paddingTop.size(),
                 0 /* deviceOffset */);
  events->insert(event);

  size_t hostOffset   = 0;
  size_t deviceOffset = paddingTop.size();
  size_t devSize      = _devices.size();

  for(size_t i = 0; i < devSize; ++i) {
    auto& devicePtr = _devices[i];
    auto& buffer    = matrix.deviceBuffer(devicePtr);

    auto size = buffer.size();

    if (i == 0)         size -= paddingTop.size();
    if (i == devSize-1) size -= paddingBottom.size();

    event = devicePtr->enqueueWrite(buffer,
                                    matrix.hostBuffer().begin(),
                                    size,
                                    deviceOffset,
                                    hostOffset);
    events->insert(event);

    hostOffset += size - _overlapRadius * columnCount;

    // offset += (buffer.size()-2*_overlap_radius*_size.column_count-deviceoffset);
    deviceOffset = 0; // after the first device, the device offset is 0
   }

   // Upload bottom padding at the end of last device
   auto& lastDevicePtr  = _devices.back();
   // calculate offset on the device ...
   deviceOffset = matrix.deviceBuffer(lastDevicePtr).size() - paddingBottom.size();

   event = lastDevicePtr->enqueueWrite(
              matrix.deviceBuffer(lastDevicePtr),
              paddingBottom.begin(),
              paddingBottom.size(),
              deviceOffset );
   events->insert(event);
}

template <typename T>
void OverlapDistribution< Vector<T> >::startDownload(Vector<T>& vector,
                                                     Event* events) const
{
  ASSERT(events != nullptr);

  size_t offset = 0;

  for (auto& devicePtr : _devices) {
    auto& buffer = vector.deviceBuffer(devicePtr);

    auto& size = buffer.size - (2 * _overlapRadius);

    auto event = devicePtr->enqueueRead(buffer,
                                        vector.hostBuffer().begin(),
                                        size,
                                        _overlapRadius,
                                        offset);
    offset += size;
    events->insert(event);
  }

  // mark data on device as out of date !
  // TODO: find out why? -> ask matthias
  matrix.dataOnHostModified();
}

template <typename T>
void OverlapDistribution< Matrix<T> >::startDownload(Matrix<T>& matrix,
                                                     Event* events) const
{
  ASSERT(events != nullptr);

  size_t offset = 0;

  for (auto& devicePtr : _devices) {
    auto& buffer = matrix.deviceBuffer(devicePtr);

    auto& overlapSize = _overlapRadius * matrix.size().columnCount();

    auto& size =   buffer.size() - (2 * overlapSize);

    auto event = devicePtr->enqueueRead(buffer,
                                        matrix.hostBuffer().begin(),
                                        size,
                                        overlapSize,
                                        offset);
    offset += size;
    events->insert(event);
  }

  // mark data on device as out of date !
  // TODO: find out why? -> ask matthias
  matrix.dataOnHostModified();
}

template <typename T>
size_t OverlapDistribution< Matrix<T> >::sizeForDevice(Vector<T>& vector,
                                                       const detail::Device::id_type id) const
{
  if (id < _devices.size() - 1) {
    auto s = vector.size() / _devices.size();
    s += _overlapRadius + _overlapRadius;
    return s;

  } else { // "last" device

    auto s = vector.size() / _devices.size();
    s += vector.size() % _devices.size();
    s += _overlapRadius + _overlapRadius;
    return s;
  }
}

template <typename T>
size_t OverlapDistribution< Matrix<T> >::sizeForDevice(Matrix<T>& matrix,
                                                       const detail::Device::id_type id) const
{
  if (id < _devices.size() - 1) {
    auto s = matrix.size().rowCount() / _devices.size();
    s += _overlapRadius + _overlapRadius;
    return s * size.columnCount();

  } else { // "last" device

    auto s = matrix.size().rowCount() / _devices.size();
    s += matrix.size().rowCount() % _devices.size();
    s += _overlapRadius + _overlapRadius;
    return s * size.columnCount();
  }
}

template <typename T>
bool OverlapDistribution< Vector<T> >::dataExchangeOnDistributionChange(
                                Distribution< Vector<T> >& /*newDistribution*/)
{
  return true;
}

template <typename T>
bool OverlapDistribution< Matrix<T> >::dataExchangeOnDistributionChange(
                                Distribution< Matrix<T> >& /*newDistribution*/)
{
  return true;
}

template <typename T>
typename Vector<T>::size_type& Vector<T>::overlapRadius() const
{
  return _overlapRadius;
}

template <typename T>
typename Matrix<T>::size_type::size_type& Matrix<T>::overlapRadius() const
{
  return _overlapRadius;
}

template <typename T>
Padding& Vector<T>::padding() const
{
  return _padding;
}

template <typename T>
Padding& Matrix<T>::padding() const
{
  return _padding;
}

template <typename T>
T& Vector<T>::neutralElement() const
{
  return _neutralElement;
}

template <typename T>
T& Matrix<T>::neutralElement() const
{
  return _neutralElement;
}

template <typename T>
bool OverlapDistribution< Vector<T> >::doCompare(const Distribution< Vector<T> >& rhs) const
{
  bool ret = false;
  // can rhs be casted into overlap distribution ?
  auto const overlapRhs = dynamic_cast<const OverlapDistribution< Vector<T> >*>(&rhs);
  if (overlapRhs) {
    ret = true;
  }
  return ret;
}

template <typename T>
bool OverlapDistribution< Matrix<T> >::doCompare(const Distribution< Matrix<T> >& rhs) const
{
  bool ret = false;
  // can rhs be casted into overlap distribution ?
  auto const overlapRhs = dynamic_cast<const OverlapDistribution< Matrix<T> >*>(&rhs);
  if (overlapRhs) {
    ret = true;
  }
  return ret;
}

} // namespace detail

} // namespace skelcl

