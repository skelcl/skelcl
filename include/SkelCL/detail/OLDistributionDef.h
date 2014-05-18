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
/// \file OLDistributionDef.h
///
/// Working for Matrix-Version.
///
///	\author Stefan Breuer <s_breu03@uni-muenster.de>
///
#ifndef OL_DISTRIBUTION_DEF_H_
#define OL_DISTRIBUTION_DEF_H_

#include <pvsutil/Logger.h>

namespace skelcl {

namespace detail {

template <template <typename> class C, typename T>
OLDistribution<C<T>>::OLDistribution(const unsigned int overlapRadius,
                                     const detail::Padding padding,
                                     const T neutral_element,
                                     const DeviceList& deviceList)
  : Distribution<C<T>>(deviceList), _overlap_radius(overlapRadius),
    _padding(padding), _neutral_element(neutral_element)
{
}

template <template <typename> class C, typename T>
template <typename U>
OLDistribution<C<T>>::OLDistribution(const OLDistribution<C<U>>& rhs)
  : Distribution<C<T>>(rhs), _overlap_radius(rhs.getOverlapRadius()),
    _padding(rhs.getPadding()), _neutral_element()
//_neutral_element(rhs.getNeutralElement()) // TODO: find solution for this!
{
}

template <template <typename> class C, typename T>
OLDistribution<C<T>>::~OLDistribution()
{
}

template <template <typename> class C, typename T>
bool OLDistribution<C<T>>::isValid() const
{
  return true;
}

template <template <typename> class C, typename T>
void OLDistribution<C<T>>::startUpload(C<T>& container, Event* events) const
{
  ASSERT(events != nullptr);
  ol_distribution_helper::startUpload(container, events, this->_overlap_radius,
                                      this->_padding, this->_neutral_element,
                                      this->_devices);
}

template <template <typename> class C, typename T>
void OLDistribution<C<T>>::startDownload(C<T>& container, Event* events) const
{
  ASSERT(events != nullptr);
  ol_distribution_helper::startDownload(container, events,
                                        this->_overlap_radius, this->_devices);
}

template <template <typename> class C, typename T>
size_t OLDistribution<C<T>>::sizeForDevice(
    const C<T>& container,
    const std::shared_ptr<detail::Device>& devicePtr) const
{
  return ol_distribution_helper::sizeForDevice<T>(
      devicePtr, container.size(), this->_devices, this->_overlap_radius);
}

template <template <typename> class C, typename T>
bool OLDistribution<C<T>>::dataExchangeOnDistributionChange(
    Distribution<C<T>>& newDistribution)
{
  auto block = dynamic_cast<OLDistribution<C<T>>*>(&newDistribution);

  if (block == nullptr) { // distributions differ => data exchange
    return true;
  } else { // new distribution == block distribution
    if (this->_devices == block->_devices) // same set of devices
    {
      return false; // => no data exchange
    } else {
      return true; // => data exchange
    }
  }
}

template <template <typename> class C, typename T>
const unsigned int& OLDistribution<C<T>>::getOverlapRadius() const
{
  return this->_overlap_radius;
}

template <template <typename> class C, typename T>
const detail::Padding& OLDistribution<C<T>>::getPadding() const
{
  return this->_padding;
}

template <template <typename> class C, typename T>
const T& OLDistribution<C<T>>::getNeutralElement() const
{
  return this->_neutral_element;
}

template <template <typename> class C, typename T>
bool OLDistribution<C<T>>::doCompare(const Distribution<C<T>>& rhs) const
{
  bool ret = false;
  // can rhs be casted into block distribution ?
  auto const blockRhs = dynamic_cast<const OLDistribution*>(&rhs);
  if (blockRhs) {
    ret = true;
  }
  return ret;
}

namespace ol_distribution_helper {

template <typename T>
size_t sizeForDevice(const std::shared_ptr<Device>& devicePtr,
                     const typename Vector<T>::size_type size,
                     const DeviceList& devices, unsigned int overlapRadius)
{
  auto id = devicePtr->id();
  if (id < devices.size() - 1) {
    auto s = size / devices.size();
    s += overlapRadius + overlapRadius;
    return s;
  } else { // "last" device
    auto s = size / devices.size();
    s += size % devices.size();
    s += overlapRadius + overlapRadius;
    return s;
  }
}

template <typename T>
size_t sizeForDevice(const std::shared_ptr<Device>& devicePtr,
                     const typename Matrix<T>::size_type size,
                     const DeviceList& devices,
                     unsigned int overlapRadius)
{
  auto id = devicePtr->id();
  if (id < devices.size() - 1) {
    auto s = size.rowCount() / devices.size();
    s += overlapRadius + overlapRadius;
    return s * size.columnCount();
  } else { // "last" device
    auto s = size.rowCount() / devices.size();
    s += size.rowCount() % devices.size();
    s += overlapRadius + overlapRadius;
    return s * size.columnCount();
  }
}

template <typename T>
void startUpload(Vector<T>& vector, Event* events, unsigned int overlapRadius,
                 detail::Padding padding, const T& neutralElement,
                 const detail::DeviceList& devices)
{

  ASSERT(events != nullptr);

  // create vectors for padding elements
  std::vector<T> paddingFront;
  std::vector<T> paddingBack;

  switch (padding) {
  case Padding::NEAREST:
    paddingFront.resize(overlapRadius, vector.front());
    paddingBack.resize(overlapRadius, vector.back());
    break;
  case Padding::NEUTRAL:
    paddingFront.resize(overlapRadius, neutralElement);
    paddingBack.resize(overlapRadius, neutralElement);
    break;
  case Padding::NEAREST_INITIAL:
    LOG_ERROR(
        "The MapOverlap skeleton works with the NEAREST and NEUTRAL mode");
    break;
  }

  // upload front padding to first device
  auto& firstDevicePtr = devices.front();
  auto frontEvent =
      firstDevicePtr->enqueueWrite(vector.deviceBuffer(*firstDevicePtr),
                                   paddingFront.begin(), paddingFront.size());

  // upload back padding at the end of last device
  auto& lastDevicePtr = devices.back();
  // calculate offset on the device ...
  auto deviceOffset =
      vector.deviceBuffer(*lastDevicePtr).size() - paddingBack.size();

  auto backEvent = lastDevicePtr->enqueueWrite(
      vector.deviceBuffer(*lastDevicePtr), paddingBack.begin(),
      paddingBack.size(), deviceOffset);

  // upload the regular data
  size_t hostOffset = 0;
  deviceOffset = paddingFront.size();

  for (size_t i = 0; i < devices.size(); ++i) {
    auto& devicePtr = devices[i];
    auto& buffer = vector.deviceBuffer(*devicePtr);

    auto size = buffer.size();
    if (i == 0) size -= paddingFront.size();
    if (i == devices.size() - 1) size -= paddingBack.size();

    auto event = devicePtr->enqueueWrite(buffer, vector.hostBuffer().begin(),
                                          size, deviceOffset, hostOffset);
    events->insert(event);

    hostOffset += size - overlapRadius;
    deviceOffset = 0; // after the first device, the device offset is 0
  }

  // wait for front and back transfer to finish before releasing the memory ...
  frontEvent.wait();
  backEvent.wait();
}

template <typename T>
void startUpload(Matrix<T>& matrix, Event* events, unsigned int overlapRadius,
                 detail::Padding padding, const T& neutralElement,
                 const detail::DeviceList& devices)
{
  ASSERT(events != nullptr);

  auto columnCount = matrix.size().columnCount();
  auto newSize = overlapRadius * columnCount;

  // create padding vectors
  std::vector<T> paddingTop;
  std::vector<T> paddingBottom;

  if (padding == detail::Padding::NEUTRAL) {
    paddingTop.resize(newSize, neutralElement);
    paddingBottom.resize(newSize, neutralElement);
  }

  if (padding == detail::Padding::NEAREST) {
    paddingTop.resize(newSize);
    auto firstRowBegin = matrix.row_begin(0);
    auto firstRowEnd = matrix.row_end(0);
    for (auto i = 0u; i < overlapRadius; i++) {
      std::copy(firstRowBegin, firstRowEnd,
                paddingTop.begin() + i * columnCount);
    }

    paddingBottom.resize(newSize);
    auto lastRowBegin = matrix.row_begin(matrix.size().rowCount() - 1);
    auto lastRowEnd = matrix.row_end(matrix.size().rowCount() - 1);
    for (auto i = 0u; i < overlapRadius; i++) {
      std::copy(lastRowBegin, lastRowEnd,
                paddingBottom.begin() + i * columnCount);
    }
  }

  // upload top padding to first device
  auto& firstDevicePtr = devices.front();
  auto topEvent =
      firstDevicePtr->enqueueWrite(matrix.deviceBuffer(*firstDevicePtr),
                                   paddingTop.begin(), paddingTop.size(), 0);

  // upload bottom padding to last device
  auto& lastDevicePtr = devices.back();
  auto deviceOffset =
      matrix.deviceBuffer(*lastDevicePtr).size() - paddingBottom.size();
  auto bottomEvent = lastDevicePtr->enqueueWrite(
      matrix.deviceBuffer(*lastDevicePtr), paddingBottom.begin(),
      paddingBottom.size(), deviceOffset, 0);

  // upload the regular parts
  size_t hostOffset = 0;
  deviceOffset = paddingTop.size();

  for (size_t i = 0; i < devices.size(); ++i) {
    auto& devicePtr = devices[i];
    auto& buffer = matrix.deviceBuffer(*devicePtr);

    auto size = buffer.size();
    if (i == 0) size -= paddingTop.size();
    if (i == devices.size() - 1) size -= paddingBottom.size();
    auto event = devicePtr->enqueueWrite(buffer, matrix.hostBuffer().begin(),
                                         size, deviceOffset, hostOffset);
    events->insert(event);

    if (i == 0) {
      hostOffset += (buffer.size() - 2 * overlapRadius * columnCount -
                     overlapRadius * columnCount);
    } else {
      hostOffset += buffer.size() - 2 * overlapRadius * columnCount;
    }

    deviceOffset = 0; // after the first device, the device offset is 0
  }

  // wait for the data transfers to finish before releasing the memory of
  // paddingTop and paddingBottom
  topEvent.wait();
  bottomEvent.wait();
}

template <typename T>
void startDownload(Vector<T>& vector, Event* events, unsigned int overlapRadius,
                   const detail::DeviceList& devices)
{
  ASSERT(events != nullptr);

  size_t offset = 0;

  for (auto& devicePtr : devices) {
    auto& buffer = vector.deviceBuffer(*devicePtr);

    auto size = buffer.size() - (2 * overlapRadius);

    auto event = devicePtr->enqueueRead(buffer, vector.hostBuffer().begin(),
                                        size, overlapRadius, offset);
    offset += size;
    events->insert(event);
  }

  // mark data on device as out of date !
  // TODO: find out why? -> ask matthias
  vector.dataOnHostModified();
}

template <typename T>
void startDownload(Matrix<T>& matrix, Event* events, unsigned int overlapRadius,
                   const detail::DeviceList& devices)
{
  ASSERT(events != nullptr);

  size_t offset = 0;

  for (auto& devicePtr : devices) {
    auto& buffer = matrix.deviceBuffer(*devicePtr);

    auto overlapSize = overlapRadius * matrix.size().columnCount();

    auto size = buffer.size() - (2 * overlapSize);

    auto event = devicePtr->enqueueRead(buffer, matrix.hostBuffer().begin(),
                                        size, overlapSize, offset);
    offset += size;
    events->insert(event);
  }

  // mark data on device as out of date !
  // TODO: find out why? -> ask matthias
  matrix.dataOnHostModified();
}

} // namespace ol_distribution_helper

} // namespace detail

} // namespace skelcl

#endif // OL_DISTRIBUTION_DEF _H_
