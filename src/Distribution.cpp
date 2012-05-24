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
/// Distribution.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "SkelCL/Distribution.h"

#include "SkelCL/detail/BlockDistribution.h"
#include "SkelCL/detail/CopyDistribution.h"
#include "SkelCL/detail/Device.h"
#include "SkelCL/detail/DeviceList.h"
#include "SkelCL/detail/Logger.h"
#include "SkelCL/detail/Significances.h"
#include "SkelCL/detail/SingleDistribution.h"
#include "SkelCL/detail/Util.h"

namespace skelcl {

Distribution::Distribution(const Distribution& rhs)
  : _devices(rhs._devices)
{
  LOG_DEBUG("Created new Distribution object (", this, ") by copying (",
            &rhs, ") with ", getInfo());
}

Distribution& Distribution::operator=(const Distribution& rhs)
{
  if (this == &rhs) return *this; // handle self assignment
  _devices  = rhs._devices;
  LOG_DEBUG("Assignment to Distribution object (", this, ") now with ",
            getInfo());
  return *this;
}

Distribution::Distribution(const detail::DeviceList& deviceList)
  : _devices(deviceList)
{
  LOG_DEBUG("Created new Distribution object (", this, ") with ", getInfo());
}

Distribution::~Distribution()
{
  LOG_DEBUG("Distribution object (", this, ") with ", getInfo(), " destroyed");
}

std::shared_ptr<Distribution> Distribution::Single(const detail::Device::id_type deviceID)
{
  return std::make_shared<detail::SingleDistribution>(detail::globalDeviceList[deviceID]);
}

std::shared_ptr<Distribution> Distribution::Block()
{
  return std::make_shared<detail::BlockDistribution>(detail::globalDeviceList);
}

std::shared_ptr<Distribution>
  Distribution::Block(const detail::DeviceList& deviceList,
                      const detail::Significances& significances)
{
  return std::make_shared<detail::BlockDistribution>(deviceList, significances);
}

std::shared_ptr<Distribution> Distribution::Copy()
{
  return std::make_shared<detail::CopyDistribution<void> >(detail::globalDeviceList);
}

bool Distribution::operator==(const Distribution& rhs) const
{
  LOG_DEBUG("Compare Distribution objects (this: [", this,
             "], rhs: [", &rhs, "])");
  return this->doCompare(rhs) && rhs.doCompare(*this);
}

bool Distribution::doCompare(const Distribution& rhs) const
{
  LOG_DEBUG("doCompare");
  return (_devices == rhs._devices);
}

bool Distribution::operator!=(const Distribution& rhs) const
{
  return !operator==(rhs);
}

bool Distribution::isSingle() const
{
  return false;
}

bool Distribution::isBlock() const
{
  return false;
}

bool Distribution::isCopy() const
{
  return false;
}

bool Distribution::isValid() const
{
  return ( this->isSingle() || this->isBlock() || this->isCopy() );
}

const detail::DeviceList& Distribution::devices() const
{
  return _devices;
}

const detail::DeviceList::value_type
  Distribution::device(const size_t device) const
{
  ASSERT(device < _devices.size());
  return _devices[device];
}

std::string Distribution::getInfo() const
{
  std::stringstream s;
  s << "type: ";
       if (isSingle()) { s << "single";  }
  else if (isBlock() ) { s << "block";   }
  else if (isCopy()  ) { s << "copy";    }
  else                 { s << "invalid"; }
  s << ", #devices: " << _devices.size();
  return s.str();
}

} // namespace skelcl
