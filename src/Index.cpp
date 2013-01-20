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
/// \file Index.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include "SkelCL/Index.h"

namespace skelcl {

Index::Index()
  : _index(0)
{
}

Index::Index(const index_type index)
  : _index(index)
{
}

bool Index::operator==(const Index& rhs) const
{
  return _index == rhs._index;
}

bool Index::operator!=(const Index& rhs) const
{
  return _index != rhs._index;
}

Index::operator index_type() const
{
  return _index;
}

Index::index_type Index::get() const
{
  return _index;
}

IndexPoint::IndexPoint()
  : _indexPoint()
{
}

IndexPoint::IndexPoint(const indexPoint_type& indexPoint)
  : _indexPoint(indexPoint)
{
}

IndexPoint::IndexPoint(const Index& row, const Index& column)
  : _indexPoint(std::make_pair(row,column))
{
}

IndexPoint::IndexPoint(IndexPoint&& rhs)
  : _indexPoint(std::make_pair(std::move(rhs._indexPoint.first),
                               std::move(rhs._indexPoint.second)))
{
}

IndexPoint& IndexPoint::operator=(IndexPoint&& rhs)
{
  if (this == &rhs) return *this; // handle self move assignment
  _indexPoint.first   = std::move(rhs._indexPoint.first);
  _indexPoint.second  = std::move(rhs._indexPoint.second);
  return *this;
}

bool IndexPoint::operator==(const IndexPoint& rhs) const
{
  return _indexPoint == rhs._indexPoint;
}

bool IndexPoint::operator!=(const IndexPoint& rhs) const
{
  return !(this->operator==(rhs));
}

IndexPoint::operator indexPoint_type() const
{
  return _indexPoint;
}

IndexPoint::indexPoint_type IndexPoint::get() const
{
  return _indexPoint;
}

const Index& IndexPoint::rowID() const
{
  return _indexPoint.first;
}
  
const Index& IndexPoint::y() const
{
  return _indexPoint.first;
}

const Index& IndexPoint::columnID() const
{
  return _indexPoint.second;
}
  
const Index& IndexPoint::x() const
{
  return _indexPoint.second;
}

} // namespace skelcl

