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
///  IndexMatrix.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <algorithm>
#include <ios>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>

#include <SkelCL/IndexMatrix.h>
#include <SkelCL/Distributions.h>

#include <SkelCL/detail/Device.h>
#include <SkelCL/detail/DeviceBuffer.h>
#include <SkelCL/detail/DeviceList.h>
#include <SkelCL/detail/Distribution.h>
#include <SkelCL/detail/Event.h>

namespace skelcl {

Matrix<IndexPoint>::Matrix(const size_type size)
  : _maxIndex{size.rowCount() - 1, size.columnCount() - 1},
    _distribution(new skelcl::detail::Distribution<Matrix<IndexPoint>>())
{
  LOG_DEBUG_INFO("Created new IndexMatrix object (", this, ") with ",
                 getDebugInfo());
}

Matrix<IndexPoint>::~Matrix()
{
  LOG_DEBUG_INFO("IndexMatrix object (", this, ") with ", getDebugInfo(),
                 " destroyed");
}

//  template <>
//  const_iterator Matrix<IndexPoint>::begin() const
//  {
//  }

//  template <>
//  const_iterator Matrix<IndexPoint>::end() const;
//  {
//  }

Matrix<IndexPoint>::size_type Matrix<IndexPoint>::size() const
{
  return {_maxIndex.rowID() + 1, _maxIndex.columnID() + 1};
}

detail::Sizes Matrix<IndexPoint>::sizes() const
{
  ASSERT(_distribution != nullptr);

  detail::Sizes s;
  for (auto& devicePtr : _distribution->devices()) {
    s.push_back(this->_distribution->sizeForDevice(*this, devicePtr));
  }
  return s;
}

Matrix<IndexPoint>::value_type Matrix<IndexPoint>::operator[](size_type n) const
{
  return {n.rowCount(), n.columnCount()};
}

Matrix<IndexPoint>::value_type Matrix<IndexPoint>::at(size_type n) const
{
  if (std::make_tuple(n.rowCount(), n.columnCount()) >=
      std::make_tuple(_maxIndex.rowID(), _maxIndex.columnID())) {
    throw std::out_of_range("Out of range access.");
  }
  return {n.rowCount(), n.columnCount()};
}

Matrix<IndexPoint>::value_type Matrix<IndexPoint>::front() const
{
  return {0, 0};
}

Matrix<IndexPoint>::value_type Matrix<IndexPoint>::back() const
{
  return _maxIndex;
}

detail::Distribution<Matrix<IndexPoint>>&
    Matrix<IndexPoint>::distribution() const
{
  ASSERT(_distribution != nullptr);
  return *_distribution;
}

void Matrix<IndexPoint>::setDistribution(std::unique_ptr<
    detail::Distribution<Matrix<IndexPoint>>>&& newDistribution) const
{
  ASSERT(newDistribution != nullptr);
  ASSERT(newDistribution->isValid());

  _distribution = std::move(newDistribution);

  ASSERT(_distribution->isValid());

  LOG_DEBUG_INFO("IndexMatrix object (", this,
                 ") assigned new distribution, now with ", getDebugInfo());
}

std::string Matrix<IndexPoint>::deviceFunctions()
{
  return std::string();
}

std::string Matrix<IndexPoint>::getInfo() const
{
  std::stringstream s;
  s << "size: {" << size().rowCount() << ", " << size().columnCount() << "}";
  return s.str();
}

std::string Matrix<IndexPoint>::getDebugInfo() const
{
  std::stringstream s;
  s << getInfo();
  return s.str();
}

const detail::DeviceBuffer&
Matrix<IndexPoint>::deviceBuffer(const detail::Device& /*device*/) const
{
  ASSERT_MESSAGE(false, "This function should never be called!");
  static detail::DeviceBuffer db;
  return db;
}

std::vector<IndexPoint>& Matrix<IndexPoint>::hostBuffer() const
{
  ASSERT_MESSAGE(false, "This function should never be called!");
  static std::vector<IndexPoint> v;
  return v;
}

void Matrix<IndexPoint>::dataOnDeviceModified() const
{
  ASSERT_MESSAGE(false, "This function should never be called!");
}

void Matrix<IndexPoint>::dataOnHostModified() const
{
  ASSERT_MESSAGE(false, "This function should never be called!");
}
  
} // namespace skelcl


