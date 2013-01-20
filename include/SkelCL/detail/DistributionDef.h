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
/// \file DistributionDef.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef DISTRIBUTION_DEF_H_
#define DISTRIBUTION_DEF_H_

#include <sstream>
#include <string>

#include <pvsutil/Assert.h>

#include "Device.h"
#include "DeviceList.h"
#include "Event.h"

namespace skelcl {

namespace detail {

template <template <typename> class C, typename T>
Distribution<C<T>>::Distribution()
  : _devices()
{
}

template <template <typename> class C, typename T>
template <typename U>
Distribution<C<T>>::Distribution(const Distribution<C<U>>& rhs)
  : _devices(rhs.devices())
{
}

template <template <typename> class C, typename T>
template <typename U>
Distribution<C<T>>::Distribution(Distribution<C<U>>&& rhs)
  : _devices(std::move(rhs._devices))
{
}

template <template <typename> class C, typename T>
Distribution<C<T>>::~Distribution()
{
}

template <template <typename> class C, typename T>
template <typename U>
Distribution<C<T>>& Distribution<C<T>>::operator=(const Distribution<C<U>>& rhs)
{
  _devices = rhs._devices;
}

template <template <typename> class C, typename T>
template <typename U>
Distribution<C<T>>& Distribution<C<T>>::operator=(Distribution<C<U>>&& rhs)
{
  _devices = std::move(rhs._devices);
}

template <template <typename> class C, typename T>
bool Distribution<C<T>>::operator==(const Distribution& rhs) const
{
  return this->doCompare(rhs) && rhs.doCompare(*this);
}

template <template <typename> class C, typename T>
bool Distribution<C<T>>::operator!=(const Distribution& rhs) const
{
  return !(this->operator==(rhs));
}

template <template <typename> class C, typename T>
bool Distribution<C<T>>::isValid() const
{
  return false;
}

template <template <typename> class C, typename T>
void Distribution<C<T>>::startUpload(C<T>& /*container*/,
                                     detail::Event* /*events*/) const
{
}

template <template <typename> class C, typename T>
void Distribution<C<T>>::startDownload(C<T>& /*container*/,
                                       detail::Event* /*events*/) const
{
}

template <template <typename> class C, typename T>
const detail::DeviceList& Distribution<C<T>>::devices() const
{
  return _devices;
}

template <template <typename> class C, typename T>
const detail::DeviceList::value_type
  Distribution<C<T>>::device(const size_t device) const
{
  ASSERT(device < _devices.size());
  return _devices[device];
}

template <template <typename> class C, typename T>
size_t
  Distribution<C<T>>::sizeForDevice(const C<T>& /*container*/,
                                    const std::shared_ptr<detail::Device>&
                                        /*d*/) const
{
  return 0;
}

template <template <typename> class C, typename T>
bool Distribution<C<T>>::dataExchangeOnDistributionChange(
                                   Distribution<C<T>>& /*newDistribution*/)
{
  return false;
}

template <template <typename> class C, typename T>
Distribution<C<T>>::Distribution(const detail::DeviceList& deviceList)
  : _devices(deviceList)
{
}

template <template <typename> class C, typename T>
std::string Distribution<C<T>>::getInfo() const
{
  std::stringstream s;
    s << "type: ";
    if (!isValid()) { s << "invalid"; }
    s << ", #devices: " << _devices.size();
    return s.str();
}

template <template <typename> class C, typename T>
bool Distribution<C<T>>::doCompare(const Distribution& rhs) const
{
  return (_devices == rhs._devices);
}

} // namespace detail

} // namespace skelcl

#endif // DISTRIBUTION_DEF_H_
