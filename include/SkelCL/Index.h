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
/// \file Index.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef INDEX_H_
#define INDEX_H_

#include <cstring>
#include <utility>

namespace skelcl {

class Index {
public:
  typedef size_t index_type;

  Index();

  Index(const index_type index);
  
  Index(const Index& rhs) = default;
  Index(Index&& rhs) = default;
  Index& operator=(const Index& rhs) = default;
  Index& operator=(Index&& rhs) = default;
  ~Index() = default;

  bool operator==(const Index& rhs) const;
  bool operator!=(const Index& rhs) const;

  operator index_type() const;
  index_type get() const;

private:
  index_type _index;
};

class IndexPoint {
public:
  typedef std::pair<Index, Index> indexPoint_type;

  IndexPoint() = default;

  IndexPoint(const indexPoint_type& indexPoint);
  IndexPoint(const Index& x, const Index& y);

  IndexPoint(const IndexPoint& rhs) = default;
  IndexPoint(IndexPoint&& rhs);
  IndexPoint& operator=(const IndexPoint& rhs) = default;
  IndexPoint& operator=(IndexPoint&& rhs);
  ~IndexPoint() = default;

  bool operator==(const IndexPoint& rhs) const;
  bool operator!=(const IndexPoint& rhs) const;

  operator indexPoint_type() const;
  indexPoint_type get() const;

  const Index& rowID() const;
  const Index& y() const;

  const Index& columnID() const;
  const Index& x() const;

private:
  indexPoint_type _indexPoint;
};

} // namespace skelcl

#endif // INDEX_H_

