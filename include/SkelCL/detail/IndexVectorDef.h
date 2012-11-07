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
///  IndexVectorDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef INDEX_VECTOR_DEF_H_
#define INDEX_VECTOR_DEF_H_

#include <algorithm>
#include <ios>
#include <iterator>
#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "../Distributions.h"

#include "Assert.h"
#include "Device.h"
#include "DeviceBuffer.h"
#include "DeviceList.h"
#include "Distribution.h"
#include "Event.h"
#include "Logger.h"

namespace skelcl {

Vector<Index>::Vector(const value_type size,
                      const detail::Distribution< Vector<Index> >& distribution)
  : _maxIndex(size-1),
    _distribution(detail::cloneAndConvert<Index>(distribution))
{
  LOG_DEBUG("Created new IndexVector object (", this, ") with ", getDebugInfo());
}

Vector<Index>::Vector(const Vector<Index>& rhs)
  : _maxIndex(rhs._maxIndex),
    _distribution(detail::cloneAndConvert<Index>(rhs.distribution()))
{
  LOG_DEBUG("Created new IndexVector object (", this, ") by copying (", &rhs,
            ") with ", getDebugInfo());
}

Vector<Index>::~Vector()
{
  LOG_DEBUG("IndexVector object (", this, ") with ", getDebugInfo(), " destroyed");
}

//  template <>
//  const_iterator Vector<Index>::begin() const
//  {
//  }

//  template <>
//  const_iterator Vector<Index>::end() const;
//  {
//  }

Vector<Index>::size_type Vector<Index>::size() const
{
  return _maxIndex+1;
}

typename detail::Sizes Vector<Index>::sizes() const
{
  ASSERT(_distribution != nullptr);

  detail::Sizes s;
  for (auto& devicePtr : _distribution->devices()) {
    s.push_back(this->_distribution->sizeForDevice(*this,
                                                  devicePtr->id()));
  }
  return s;
}

Vector<Index>::value_type Vector<Index>::operator[]( size_type n ) const
{
  return n;
}

Vector<Index>::value_type Vector<Index>::at( size_type n ) const
{
  if (n >= _maxIndex) throw std::out_of_range("Out of range access.");
  return n;
}

Vector<Index>::value_type Vector<Index>::front() const
{
  return 0;
}

Vector<Index>::value_type Vector<Index>::back() const
{
  return _maxIndex;
}

detail::Distribution< Vector<Index> >& Vector<Index>::distribution() const
{
  ASSERT(_distribution != nullptr);
  return *_distribution;
}

template <typename U>
void Vector<Index>::setDistribution(const detail::Distribution< Vector<U> >& origDistribution) const
{
  ASSERT(origDistribution.isValid());
  // convert and set distribution
  this->setDistribution(detail::cloneAndConvert<Index>(origDistribution));
}

template <typename U>
void Vector<Index>::setDistribution(const std::unique_ptr<detail::Distribution< Vector<U> > >& origDistribution) const
{
  ASSERT(origDistribution != nullptr);
  ASSERT(origDistribution->isValid());
  // convert and set distribution
  this->setDistribution(detail::cloneAndConvert<Index>(*origDistribution));
}

void Vector<Index>::setDistribution(std::unique_ptr<detail::Distribution< Vector<Index> > >&& newDistribution) const
{
  ASSERT(newDistribution != nullptr);
  ASSERT(newDistribution->isValid());

  _distribution = std::move(newDistribution);
  ASSERT(_distribution->isValid());

  LOG_DEBUG("IndexVector object (", this, ") assigned new distribution, now with ",
           getDebugInfo());
}

std::string Vector<Index>::deviceFunctions()
{
  return std::string();
}

std::string Vector<Index>::getInfo() const
{
  std::stringstream s;
  s << "size: "                   << size();
  return s.str();
}

std::string Vector<Index>::getDebugInfo() const
{
  std::stringstream s;
  s << getInfo();
  return s.str();
}

const detail::DeviceBuffer& Vector<Index>::deviceBuffer(const detail::Device& /*device*/) const
{
  ASSERT_MESSAGE(false, "This function should never be called!");
  static detail::DeviceBuffer db;
  return db;
}

std::vector<Index>& Vector<Index>::hostBuffer() const
{
  ASSERT_MESSAGE(false, "This function should never be called!");
  static std::vector<Index> v;
  return v;
}

void Vector<Index>::dataOnDeviceModified() const
{
  ASSERT_MESSAGE(false, "This function should never be called!");
}

void Vector<Index>::dataOnHostModified() const
{
  ASSERT_MESSAGE(false, "This function should never be called!");
}

} // namespace skelcl

#endif // INDEX_VECTOR_DEF_H_

