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

template <template <typename> class C, typename T>
CopyDistribution< C<T> >::CopyDistribution(const DeviceList& deviceList,
                                           std::function<T(const T&, const T&)>
                                             combineFunc)
  : Distribution< C<T> >(deviceList), _combineFunc(combineFunc)
{
}

template <template <typename> class C, typename T>
template <typename U>
CopyDistribution< C<T> >::CopyDistribution( const CopyDistribution< C<U> >& rhs)
  : Distribution< C<T> >(rhs), _combineFunc(nullptr)
{
  // TODO: allow this? How handle this?
}

template <template <typename> class C, typename T>
CopyDistribution< C<T> >::~CopyDistribution()
{
}

template <template <typename> class C, typename T>
bool CopyDistribution< C<T> >::isValid() const
{
  return true;
}

template <template <typename> class C, typename T>
void CopyDistribution< C<T> >::startUpload(C<T>& container,
                                           Event* events) const
{
  ASSERT(events != nullptr);

  for (auto& devicePtr : this->_devices) {

    auto& buffer = container.deviceBuffer(*devicePtr);

    auto event = devicePtr->enqueueWrite(buffer,
                                         container.hostBuffer().begin());
    events->insert(event);
  }
}

template <template <typename> class C, typename T>
void CopyDistribution< C<T> >::startDownload(C<T>& container,
                                             Event* events) const
{
  ASSERT(events != nullptr);

  if (_combineFunc == nullptr) {
    // take data from the first device

    auto& buffer = container.deviceBuffer(*(this->_devices.front()));

    auto event = this->_devices.front()->enqueueRead(buffer,
                                                     container.hostBuffer().begin());
    events->insert(event);

  } else { // using combine function

    // create temporary vectors
    std::vector<std::vector<T>> vs(this->_devices.size() - 1);
    // for every device ...
    for (auto& devicePtr : this->_devices) {
      auto& deviceBuffer = container.deviceBuffer(*devicePtr);
      // ... download data ...
      // ... the first devices write to the temporary vectors ...
      if (devicePtr->id() < this->_devices.size() - 1) {
        vs[devicePtr->id()].resize(deviceBuffer.size());
        auto event = devicePtr->enqueueRead( deviceBuffer,
                                             vs[devicePtr->id()].begin() );
        events->insert(event);
      } else { // ... the "last" device writes directly to the host buffer
        auto event = devicePtr->enqueueRead( deviceBuffer,
                                             container.hostBuffer().begin() );
        events->insert(event);
      }
    }
    events->wait(); // wait for downloads to finish

    // combine all vs into hostPointer using _combineFunc
    for (unsigned i = 0; i < vs.size(); ++i) {
      std::transform(vs[i].begin(), vs[i].end(),
                     static_cast<T*>(&(container.hostBuffer().front())),
                     static_cast<T*>(&(container.hostBuffer().front())),
                     this->_combineFunc);
    }
  }

  // mark data on device as out of date !
  container.dataOnHostModified();
}

template <template <typename> class C, typename T>
size_t CopyDistribution< C<T> >::sizeForDevice(C<T>& container,
                                               const detail::Device::id_type /*id*/) const
{
  return copy_distribution_helper::sizeForDevice<T>(container.size());
}

template <template <typename> class C, typename T>
bool CopyDistribution< C<T> >::dataExchangeOnDistributionChange(
                                   Distribution< C<T> >& /*newDistribution*/)
{
  return true; // always do data exchange for copy distibution
}

template <template <typename> class C, typename T>
std::function<T(const T&, const T&)> CopyDistribution< C<T> >::combineFunc() const
{
  return this->_combineFunc;
}

template <template <typename> class C, typename T>
bool CopyDistribution< C<T> >::doCompare(const Distribution< C<T> >& rhs) const
{
  bool ret = false;
  auto const copyRhs = dynamic_cast<const CopyDistribution< C<T> >*>(&rhs);
  if (copyRhs) {
    ret = true;
  }
  return ret;
}

namespace copy_distribution_helper {

template <typename T>
size_t sizeForDevice(const typename Vector<T>::size_type size)
{
  return size;
}

} // namespace copy_distribution_helper

} // namespace detail

} // namespace skelcl

#endif // COPY_DISTRIBUTION_DEF_H_
