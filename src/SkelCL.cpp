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
/// \file SkelCL.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include "SkelCL/SkelCL.h"

#include "SkelCL/detail/DeviceList.h"
#include "SkelCL/detail/DeviceProperties.h"
#include "SkelCL/detail/Logger.h"

namespace skelcl {

void init(detail::DeviceProperties properites)
{
  detail::globalDeviceList.init(std::move(properites));
}

detail::DeviceProperties allDevices()
{
  return detail::DeviceProperties::allDevices();
}

detail::DeviceProperties nDevices(size_t n)
{
  return detail::DeviceProperties::nDevices(n);
}

void terminate()
{
  detail::globalDeviceList.clear();
  LOG_INFO("SkelCL terminating. Releasing all devices.");
}

} // namespace skelcl

