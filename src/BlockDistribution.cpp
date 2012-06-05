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
/// \file BlockDistribution.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include "SkelCL/detail/BlockDistribution.h"

#include "SkelCL/Distribution.h"

#include "SkelCL/detail/Assert.h"
#include "SkelCL/detail/DeviceList.h"
#include "SkelCL/detail/Significances.h"

namespace skelcl {

namespace detail {

BlockDistribution::BlockDistribution(const DeviceList& deviceList)
  : skelcl::Distribution(deviceList), _significances(deviceList.size())
{
}

BlockDistribution::BlockDistribution(const DeviceList& deviceList,
                                     const Significances& significances)
  : skelcl::Distribution(deviceList),
    _significances(significances)
{
  ASSERT(_devices.size() == _significances.size());
}

BlockDistribution::~BlockDistribution()
{
}

bool BlockDistribution::isBlock() const
{
  return true;
}

void BlockDistribution::startUpload(
                const std::vector<detail::DeviceBuffer>& deviceBuffers,
                void* const hostPointer,
                detail::Event* events) const
{
  ASSERT(events != nullptr);

  size_t offset = 0;
  for (auto& devicePtr : _devices) {
    auto event = devicePtr->enqueueWrite(
                                deviceBuffers[devicePtr->id()],
                                hostPointer,
                                offset);
    offset += deviceBuffers[devicePtr->id()].size();
    events->insert(event);
  }
}

void BlockDistribution::startDownload(
                const std::vector<detail::DeviceBuffer>& deviceBuffers,
                void* const hostPointer,
                detail::Event* events) const
{
  ASSERT(events != nullptr);

  size_t offset = 0;
  for (auto& devicePtr : _devices) {
    auto event = devicePtr->enqueueRead(
                               deviceBuffers[devicePtr->id()],
                               hostPointer,
                               offset);
    offset += deviceBuffers[devicePtr->id()].size();
    events->insert(event);
  }
}

size_t BlockDistribution::sizeForDevice(
        const detail::Device::id_type deviceID,
        const size_t totalSize) const
{
  if (deviceID < _devices.size()-1) {
    return ( totalSize * _significances.getSignificance(deviceID) );
  } else { // "last" device
    size_t s = totalSize * _significances.getSignificance(deviceID);
    // add rest ...
    size_t r = totalSize;
    for (const auto& devicePtr : _devices) {
      r -= totalSize * _significances.getSignificance(devicePtr->id());
    }
    return (s+r);
  }
}


bool BlockDistribution::doCompare(const Distribution& rhs) const
{
  bool ret = false;
  auto const blockRhs = dynamic_cast<const BlockDistribution*>(&rhs);
  if (blockRhs) {
    ret = true;
  }
  return ret;
}

const Significances& BlockDistribution::getSignificances() const
{
  return _significances;
}

} // namespace detail

} // namespace skelcl

