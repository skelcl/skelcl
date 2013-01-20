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

/// 
/// \brief  This class defines an Index, i.e. an unsigned integer representing a
///         value in a one-dimensional index space.
/// 
class Index {
public:
  /// 
  /// \brief  Actual type used to store the unsigned integer representing the
  ///         Index.
  /// 
  typedef size_t index_type;

  /// 
  /// \brief Creates an Index with the value 0.
  /// 
  Index();

  /// 
  /// \brief Creates an Index with the given value.
  ///
  /// \param index The value is used to create the Index.
  /// 
  Index(const index_type index);
  
  Index(const Index& rhs) = default;
  Index(Index&& rhs) = default;
  Index& operator=(const Index& rhs) = default;
  Index& operator=(Index&& rhs) = default;
  ~Index() = default;

  /// 
  /// \brief Equality operator. Two Index objects are equal if and only if the
  ///        two underlying unsigned integer are equal.
  ///
  /// \param rhs The Index to be compared with.
  ///
  /// \return True if and only if the two underlying unsigned integers are
  ///         equal. False otherwise.
  /// 
  bool operator==(const Index& rhs) const;

  /// 
  /// \brief Inequality operator. Two Index objects are not equal if and only if
  ///        the two underlying unsigned integer are not equal.
  ///
  /// \param rhs The Index to be compared with.
  ///
  /// \return True if and only if the two underlying unsigned integers are
  ///         not equal. False otherwise.
  /// 
  bool operator!=(const Index& rhs) const;

  /// 
  /// \brief Conversion operator. The Index object is automatically converted in
  ///        the underlying unsigned integer representing this Index object.
  ///
  /// \return The unsigned integer representing this Index object.
  /// 
  operator index_type() const;

  /// 
  /// \brief Explicit function to access the underlying unsigned integer
  ///        representing this Index object.
  ///
  /// \return The unsigned integer representing this Index object.
  /// 
  index_type get() const;

private:
  index_type _index;
};


/// 
/// \brief This class defines an two-dimensional IndexPoint, i.e. a pair of
///        unsigned integers representing a value in a two-dimensional index
///        space.
/// 
class IndexPoint {
public:
  /// 
  /// \brief Actual type used to store the pair of unsigned integers
  ///        representing the IndexPoint.
  /// 
  typedef std::pair<Index, Index> indexPoint_type;

  /// 
  /// \brief Creates an IndexPoint with the values {0,0}.
  /// 
  IndexPoint();

  /// 
  /// \brief Copy constructor. Creates an IndexPoint with the same values as the
  ///        given IndexPoint.
  ///
  /// \param indexPoint The IndexPoint from with the values are used to create
  ///                   the new IndexPoint.
  /// 
  IndexPoint(const indexPoint_type& indexPoint);

  /// 
  /// \brief Creates an IndexPoint with the given values so that the IndexPoint
  ///        {row,column} is created.
  ///
  /// \param row    The value is used as first value of the pair.
  /// \param column The value is used as second value of the pair.
  /// 
  IndexPoint(const Index& row, const Index& column);

  /// 
  /// \brief Move constructor. Creates an IndexPoint with the same values as the
  ///        given IndexPoint.
  ///
  /// \param indexPoint The IndexPoint from with the values are used to create
  ///                   the new IndexPoint.
  /// 
  IndexPoint(IndexPoint&& rhs);
  IndexPoint(const IndexPoint& rhs) = default;
  IndexPoint& operator=(const IndexPoint& rhs) = default;
  IndexPoint& operator=(IndexPoint&& rhs);
  ~IndexPoint() = default;

  /// 
  /// \brief Equality operator. Two IndexPoint objects are equal if and only if
  ///        the two underlying pairs of unsigned integer are equal.
  ///
  /// \param rhs The IndexPoint to be compared with.
  ///
  /// \return True if and only if the two underlying pairs of unsigned integers
  ///         are equal (i.e. both components are equal). False otherwise.
  /// 
  bool operator==(const IndexPoint& rhs) const;

  /// 
  /// \brief Inequality operator. Two IndexPoint objects are not equal if and
  ///        only if the two underlying pairs of unsigned integer are not equal.
  ///
  /// \param rhs The IndexPoint to be compared with.
  ///
  /// \return True if and only if the two underlying pairs of unsigned integers
  ///         are not equal. False otherwise.
  /// 
  bool operator!=(const IndexPoint& rhs) const;

  /// 
  /// \brief Conversion operator. The IndexPoint object is automatically\
  ///        converted in the underlying pair of unsigned integer representing
  ///        this IndexPoint object.
  ///
  /// \return The pair of unsigned integer representing this IndexPoint object.
  /// 
  operator indexPoint_type() const;

  /// 
  /// \brief Explicit function to access the underlying pair of unsigned integer
  ///        representing this IndexPoint object.
  ///
  /// \return The pair of unsigned integer representing this IndexPoint object.
  /// 
  indexPoint_type get() const;

  /// 
  /// \brief Function to access the first component of the underlying pair of
  ///        unsigned integers representing this IndexPoint object.
  ///
  /// \return The first component of the pair of unsigned integers representing
  ///         this IndexPoint object.
  /// 
  const Index& rowID() const;

  /// 
  /// \brief Function to access the first component of the underlying pair of
  ///        unsigned integers representing this IndexPoint object.
  ///
  /// \return The first component of the pair of unsigned integers representing
  ///         this IndexPoint object.
  /// 
  const Index& y() const;

  /// 
  /// \brief Function to access the second component of the underlying pair of
  ///        unsigned integers representing this IndexPoint object.
  ///
  /// \return The second component of the pair of unsigned integers representing
  ///         this IndexPoint object.
  /// 
  const Index& columnID() const;

  /// 
  /// \brief Function to access the second component of the underlying pair of
  ///        unsigned integers representing this IndexPoint object.
  ///
  /// \return The second component of the pair of unsigned integers representing
  ///         this IndexPoint object.
  /// 
  const Index& x() const;

private:
  indexPoint_type _indexPoint;
};

} // namespace skelcl

#endif // INDEX_H_

