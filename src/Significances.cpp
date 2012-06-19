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
/// \file Significances.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <initializer_list>
#include <numeric>
#include <vector>

#include "SkelCL/detail/Assert.h"
#include "SkelCL/detail/Device.h"

#include "SkelCL/detail/Significances.h"

namespace skelcl {

namespace detail {

Significances::Significances(size_t deviceCount)
  : _values(deviceCount)
{
  const value_type one  = 1.0;
  auto amount = one / deviceCount;
  auto rest   = one;
  for(auto& value : _values) {
    value = amount;
    rest -= amount;
  }
  _values[deviceCount-1] += rest;

#ifndef NDEBUG // in debug build only (to prevent unused variable warning)
  const value_type zero     = 0.0;
  const value_type epsilon  = static_cast<value_type>(1.0e-10);
  ASSERT( std::accumulate( _values.cbegin(), _values.cend(), zero ) - one < epsilon );
#endif
}

Significances::Significances(std::initializer_list<value_type> significances)
  : _values( significances.begin(), significances.end() )
{
#ifndef NDEBUG // in debug build only (to prevent unused variable warning)
  const value_type zero     = 0.0;
  const value_type one      = 1.0;
  const value_type epsilon  = static_cast<value_type>(1.0e-10);
  ASSERT( std::accumulate( _values.cbegin(), _values.cend(), zero ) - one < epsilon );
#endif
}

void Significances::setSignificances(std::initializer_list<value_type> significances)
{
  _values.assign( significances.begin(), significances.end() );
#ifndef NDEBUG // in debug build only (to prevent unused variable warning)
  const value_type zero     = 0.0;
  const value_type one      = 1.0;
  const value_type epsilon  = static_cast<value_type>(1.0e-10);
  ASSERT( std::accumulate( _values.cbegin(), _values.cend(), zero ) - one < epsilon );
#endif
}

Significances::value_type
  Significances::getSignificance(const Device::id_type device) const
{
  return _values[device];
}

size_t Significances::size() const
{
  return _values.size();
}

} // namespace detail

} // namespace skelcl
