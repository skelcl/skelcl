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
/// \file BlockDistribution.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef BLOCK_DISTRIBUTION_DEF_H_
#define BLOCK_DISTRIBUTION_DEF_H_

#include "Significances.h"

namespace skelcl {

namespace detail {

template <template <typename> class C, typename T>
BlockDistribution< C<T> >::BlockDistribution(const DeviceList& deviceList)
  : Distribution< C<T> >(deviceList), _significances(deviceList.size())
{
}

template <template <typename> class C, typename T>
BlockDistribution< C<T> >::BlockDistribution(const DeviceList& deviceList,
                                             const Significances& significances)
  : Distribution< C<T> >(deviceList), _significances(significances)
{
}

template <template <typename> class C, typename T>
template <typename U>
BlockDistribution< C<T> >::BlockDistribution( const BlockDistribution< C<U> >& rhs)
  : Distribution< C<T> >(rhs), _significances(rhs.getSignificances())
{
}

template <template <typename> class C, typename T>
BlockDistribution< C<T> >::~BlockDistribution()
{
}

template <template <typename> class C, typename T>
bool BlockDistribution< C<T> >::isValid() const
{
  return true;
}

template <template <typename> class C, typename T>
void BlockDistribution< C<T> >::startUpload(C<T>& container,
                                            Event* events) const
{
  ASSERT(events != nullptr);

  size_t offset = 0;
  for (auto& devicePtr : this->_devices) {

    auto& buffer = container.deviceBuffer(*devicePtr);

    auto event = devicePtr->enqueueWrite(buffer,
                                         container.hostBuffer().begin(),
                                         offset);
    offset += buffer.size();
    events->insert(event);
  }
}

template <template <typename> class C, typename T>
void BlockDistribution< C<T> >::startDownload(C<T>& container,
                                              Event* events) const
{
  ASSERT(events != nullptr);

  size_t offset = 0;
  for (auto& devicePtr : this->_devices) {

    auto& buffer = container.deviceBuffer(*devicePtr);

    auto event = devicePtr->enqueueRead(buffer,
                                        container.hostBuffer().begin(),
                                        offset);
    offset += buffer.size();
    events->insert(event);
  }
}

template <template <typename> class C, typename T>
size_t BlockDistribution< C<T> >::sizeForDevice(C<T>& container,
                                                const detail::Device::id_type id) const
{
  return block_distribution_helper::sizeForDevice<T>(id,
                                                     container.size(),
                                                     this->_devices,
                                                     this->_significances);
}

template <template <typename> class C, typename T>
bool BlockDistribution< C<T> >::dataExchangeOnDistributionChange(
                                   Distribution< C<T> >& newDistribution)
{
  auto block = dynamic_cast<BlockDistribution< C<T> >*>(&newDistribution);

  if (block == nullptr) { // distributions differ => data exchange
    return true;
  } else { // new distribution == block distribution
    if (   this->_devices == block->_devices // same set of devices
        && this->_significances == block->_significances // same significances
       ) {
      return false; // => no data exchange
    } else {
      return true;  // => data exchange
    }
  }
}

template <template <typename> class C, typename T>
const Significances& BlockDistribution< C<T> >::getSignificances() const
{
  return this->_significances;
}

template <template <typename> class C, typename T>
bool BlockDistribution< C<T> >::doCompare(const Distribution< C<T> >& rhs) const
{
  bool ret = false;
  // can rhs be casted into block distribution ?
  auto const blockRhs = dynamic_cast<const BlockDistribution*>(&rhs);
  if (blockRhs) {
    ret = true;
  }
  return ret;
}

namespace block_distribution_helper {

template <typename T>
size_t sizeForDevice(const Device::id_type deviceID,
                     const typename Vector<T>::size_type size,
                     const DeviceList& devices,
                     const Significances& significances)
{
  if (deviceID < devices.size()-1) {
    return static_cast<size_t>(
      size * significances.getSignificance(deviceID) );
  } else { // "last" device
    size_t s = static_cast<size_t>(
      size * significances.getSignificance(deviceID) );
    // add rest ...
    size_t r = size;
    for (const auto& devicePtr : devices) {
      r -= static_cast<size_t>(
             size * significances.getSignificance(devicePtr->id()));
    }
    return (s+r);
  }
}

} // namespace block_distribution_helper

} // namespace detail

} // namespace skelcl

#endif // BLOCK_DISTRIBUTION_DEF _H_

