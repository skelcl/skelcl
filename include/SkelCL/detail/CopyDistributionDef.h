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
/// CopyDistributionDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef COPY_DISTRIBUTION_DEF_H_
#define COPY_DISTRIBUTION_DEF_H_

#include <functional>

#include "Assert.h"
#include "DeviceList.h"

namespace skelcl {

namespace detail {

template <typename T>
CopyDistribution<T>::CopyDistribution(const DeviceList& deviceList,
                                      std::function<T(const T&, const T&)>
                                        combineFunc)
  : skelcl::Distribution(deviceList), _combineFunc(combineFunc)
{
}

template <typename T>
CopyDistribution<T>::~CopyDistribution()
{
}

template <typename T>
bool CopyDistribution<T>::doCompare(const Distribution& rhs) const
{
  bool ret = false;
  auto const copyRhs = dynamic_cast<const CopyDistribution<T>*>(&rhs);
  if (copyRhs) {
    ret = true;
  }
  return ret;
}

template <typename T>
bool CopyDistribution<T>::isCopy() const
{
  return true;
}

template <typename T>
void CopyDistribution<T>::startUpload(
            const std::vector<detail::DeviceBuffer>& deviceBuffers,
            void* const hostPointer,
            detail::Event* events) const
{
  ASSERT(events != nullptr);

  for (auto& devicePtr : _devices) {
    auto event = devicePtr->enqueueWrite(
                                deviceBuffers[devicePtr->id()],
                                hostPointer);
    events->insert(event);
  }
}

template <typename T>
void CopyDistribution<T>::startDownload(
            const std::vector<detail::DeviceBuffer>& deviceBuffers,
            void* const hostPointer,
            detail::Event* events) const
{
  ASSERT(events != nullptr);

  // create temporary vectors
  std::vector<std::vector<T>> vs(_devices.size() - 1);
  // for every device ...
  for (auto& devicePtr : _devices) {
    // ... download data ...
    // ... the first devices write to the temporary vectors ...
    if (devicePtr->id() < _devices.size() - 1) {
      auto& deviceBuffer = deviceBuffers[devicePtr->id()];
      vs[devicePtr->id()].resize(deviceBuffer.size());
      auto event = devicePtr->enqueueRead( deviceBuffer,
                                           vs[devicePtr->id()].begin() );
      events->insert(event);
    } else { // ... the "last" device writes directly to hostPointer
      auto event = devicePtr->enqueueRead( deviceBuffers[devicePtr->id()],
                                           hostPointer );
      events->insert(event);
    }
  }
  events->wait(); // wait for downloads to finish

  // combine all vs into hostPointer using _combineFunc
  for (unsigned i = 0; i < vs.size(); ++i) {
    std::transform(vs[i].begin(), vs[i].end(),
                   static_cast<T*>(hostPointer),
                   static_cast<T*>(hostPointer),
                   _combineFunc);
  }
}

template <typename T>
size_t CopyDistribution<T>::sizeForDevice(
        const detail::Device::id_type /*deviceID*/,
        const size_t totalSize) const
{
  return totalSize;
}

template <typename T>
std::function<T(const T&, const T&)> CopyDistribution<T>::combineFunc() const
{
  return _combineFunc;
}

} // namespace detail

} // namespace skelcl

#endif // COPY_DISTRIBUTION_DEF_H_
