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
///  IndexVector.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef INDEX_VECTOR_H_
#define INDEX_VECTOR_H_

#include <vector>

#include "Vector.h"
#include "Index.h"

#include "detail/DeviceList.h"

namespace skelcl {

typedef const Vector<Index> IndexVector;

template <>
class Vector<Index> {
public:
  typedef Index value_type;
//  typedef ?? const_iterator;
  typedef size_t size_type;

  Vector(const value_type size);// ,
         //const detail::Distribution<Vector<Index>>& distribution
         //                           = detail::Distribution<Vector<Index>>());
  Vector(const Vector<Index>& rhs);

  ~Vector();

  // TODO: vector interface

//  const_iterator begin() const;
//  const_iterator end() const;
  size_type size() const;
  detail::Sizes sizes() const;
  value_type operator[]( size_type n ) const;
  value_type at( size_type n ) const;
  value_type front() const;
  value_type back() const;

  ///
  /// \brief Returns a pointer to the current distribution of the vector.
  /// \return A pointer to the current distribution of the vector, of nullptr
  ///         if no distribution is set
  ///
  detail::Distribution<Vector<Index>>& distribution() const;

  ///
  /// \brief Changes the distribution of the vector
  ///
  /// Changing the distribution might lead to data transfer between the host and
  /// the devices.
  ///
  /// \param distribution The new distribution to be set. After this call
  ///                     distribution is the new selected distribution of the
  ///                     vector
  ///
  template <typename U>
  void setDistribution(const detail::Distribution<Vector<U>>&
                          distribution) const;

  template <typename U>
  void setDistribution(const std::unique_ptr<detail::Distribution<Vector<U>>>&
                          newDistribution) const;

  void setDistribution(std::unique_ptr<detail::Distribution<Vector<Index>>>&&
                          newDistribution) const;

  static std::string deviceFunctions();

  //
  const detail::DeviceBuffer& deviceBuffer(const detail::Device& device) const;

  std::vector<Index>& hostBuffer() const;

  void dataOnDeviceModified() const;

  void dataOnHostModified() const;

private:
  Vector();// = delete;
  Vector(Vector<Index> && rhs);// = delete;
  Vector<Index>& operator=(const Vector<Index>&);// = delete;
  Vector<Index>& operator=(Vector<Index> && rhs);// = delete;

  ///
  /// \brief Formates information about the current instance into a string,
  ///        used for Debug purposes
  ///
  /// \return A formated string with information about the current instance
  ///
  std::string getInfo() const;

  ///
  /// \brief Formates even more information about the current instance into a
  ///        string, as compared to getInfo, used for Debug purposes
  ///
  /// \return A formated string with information about the current instance,
  ///         contains all information from getInfo and more.
  ///
  std::string getDebugInfo() const;

    value_type                                                _maxIndex;
  mutable
    std::unique_ptr<detail::Distribution<Vector<Index>>>      _distribution;
};

} // namespace skelcl

#include "detail/IndexVectorDef.h"

#endif // INDEX_VECTOR_H_

