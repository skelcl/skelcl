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
/// \file SingleDistribution.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include "SkelCL/detail/SingleDistribution.h"

#include "SkelCL/Distribution.h"
#include "SkelCL/Vector.h"

#include "SkelCL/detail/Assert.h"
#include "SkelCL/detail/Device.h"

namespace skelcl {

namespace detail {

SingleDistribution::SingleDistribution(std::shared_ptr<Device> device)
  : skelcl::Distribution( {device} )
{
}

SingleDistribution::~SingleDistribution()
{
}

bool SingleDistribution::isSingle() const
{
  return true;
}

void SingleDistribution::startUpload(
        const std::vector<detail::DeviceBuffer>& deviceBuffers,
        void* const hostPointer,
        detail::Event* events) const
{
  ASSERT(events != nullptr);

  auto event = _devices.front()->enqueueWrite(
      deviceBuffers[_devices.front()->id()],
      hostPointer );

  events->insert(event);
}

void SingleDistribution::startDownload(
        const std::vector<detail::DeviceBuffer>& deviceBuffers,
        void* const hostPointer,
        detail::Event* events) const
{
  ASSERT(events != nullptr);

  auto event = _devices.front()->enqueueRead(
        deviceBuffers[_devices.front()->id()],
        hostPointer );
  events->insert(event);
}

size_t SingleDistribution::sizeForDevice(
        const detail::Device::id_type deviceID,
        const size_t totalSize) const
{
  if (_devices.front()->id() == deviceID) {
    return totalSize;
  } else {
    return 0;
  }
}


bool SingleDistribution::doCompare(const Distribution& rhs) const
{
  bool ret = false;
  auto const singleRhs = dynamic_cast<const SingleDistribution*>(&rhs);
  if (singleRhs) {
    ret = (_devices == singleRhs->_devices);
  }
  return ret;
}

} // namespace detail

} // namespace skelcl

