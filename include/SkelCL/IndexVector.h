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
#include "detail/skelclDll.h"

namespace skelcl {

///
/// \brief An simple to use alias for the Vector<Index> class.
///
typedef const Vector<Index> IndexVector;

/// 
/// \brief The IndexVector (a.k.a. Vector<Index>) class is a special
///        implementation of a Vector with Elements of type Index.
///
/// The implementation guaranties that no data is transfered to and from the
/// devices when using this version of the Vector. OpenCL functionality (global
/// ids) is used to make the Index%s available on the device. This
/// implementation requires, that the Vector is const and can, therefore, not be
/// modified after creation.
///
/// \ingroup containers
/// \ingroup vector
/// 
template <>
class SKELCL_DLL Vector<Index> {
public:
  ///
  /// \brief Defines the type of the elements in the Vector
  ///
  typedef Index value_type;

  ///
  /// \brief Defines the type used to denote size of the Vector
  ///
  typedef size_t size_type;

  ///
  /// \brief Create a new IndexVector with the given size.
  ///
  Vector(const value_type size);

  ///
  /// \brief Create a new IndexVector with the size and distribution given by
  ///        rhs.
  ///
  Vector(const Vector<Index>& rhs);

  ~Vector();

  // TODO: finish vector interface
//  typedef ?? const_iterator;
//  const_iterator begin() const;
//  const_iterator end() const;
  // ...

  ///
  /// \brief Returns the total size of the Vector.
  ///
  /// \return The total size of the Vector.
  ///
  size_type size() const;

  ///
  /// \brief Returns the size of the parts of the Vector which are stored on
  ///        each device.
  ///
  /// \return The sizes of the parts of the Vector which are stored on each
  ///         device.
  ///
  detail::Sizes sizes() const;

  ///
  /// \brief Returns the Index at position n (i.e. n).
  ///
  /// This version does not prevent out of range accesses.
  /// If n > size() the behavior is undefined.
  ///
  /// \param n The position for which the Index should be returned.
  ///
  /// \return The Index at position n. As the Vector stores Index%es from 0 up
  ///         to its size the return value is always n.
  ///
  value_type operator[]( size_type n ) const;

  ///
  /// \brief Returns the Index at position n (i.e. n).
  ///
  /// This version does prevent out of range accesses.
  /// If n > size() an std::out_of_range exception is thrown.
  ///
  /// \param n The position for which the Index should be returned.
  ///
  /// \return The Index at position n. As the Vector stores Index%es from 0 up
  ///         to its size the return value is always n.
  ///
  value_type at( size_type n ) const;

  ///
  /// \brief Return the first Index stored in the Vector (i.e. 0).
  ///
  /// \return Always 0.
  ///
  value_type front() const;

  ///
  /// \brief Return the last Index stored in the Vector (i.e. size()-1).
  ///
  /// \return Always size()-1.
  ///
  value_type back() const;

  ///
  /// \brief Returns the current distribution of the Vector.
  //
  /// \return The current distribution of the Vector.
  ///
  detail::Distribution<Vector<Index>>& distribution() const;

  ///
  /// \brief Changes the distribution of the Vector
  ///
  /// \param distribution The new distribution to be set. After this call
  ///                     distribution is the new selected distribution of the
  ///                     Vector.
  ///
  template <typename U>
  void setDistribution(const detail::Distribution<Vector<U>>& distribution)
      const;

  ///
  /// \brief Changes the distribution of the Vector
  ///
  /// \param newDistribution The new distribution to be set. After this call
  ///                     distribution is the new selected distribution of the
  ///                     Vector.
  ///
  template <typename U>
  void setDistribution(const std::unique_ptr<detail::Distribution<Vector<U>>>&
                           newDistribution) const;

  ///
  /// \brief Changes the distribution of the Vector
  ///
  /// \param newDistribution The new distribution to be set. After this call
  ///                     distribution is the new selected distribution of the
  ///                     Vector.
  ///
  void setDistribution(std::unique_ptr<detail::Distribution<Vector<Index>>>&&
                           newDistribution) const;

  ///
  /// \brief Returns a string defining functions necessary to access the Vector
  ///        on the device. For the IndexVector this function always returns an
  ///        empty string.
  ///
  /// This functions exists for compatibility reasons with the Vector class.
  ///
  /// \return An empty string.
  ///
  static std::string deviceFunctions();

  ///
  /// \brief This function should never be called as it only exists for
  ///        compatibility reasons with the Matrix class.
  ///
  /// This function will always fail with an assertion.
  ///
  const detail::DeviceBuffer& deviceBuffer(const detail::Device& device) const;

  ///
  /// \brief This function should never be called as it only exists for
  ///        compatibility reasons with the Matrix class.
  ///
  /// This function will always fail with an assertion.
  ///
  std::vector<Index>& hostBuffer() const;

  ///
  /// \brief This function should never be called as it only exists for
  ///        compatibility reasons with the Matrix class.
  ///
  /// This function will always fail with an assertion.
  ///
  void dataOnDeviceModified() const;

  ///
  /// \brief This function should never be called as it only exists for
  ///        compatibility reasons with the Matrix class.
  ///
  /// This function will always fail with an assertion.
  ///
  void dataOnHostModified() const;

private:
  Vector();// = delete;
  Vector(Vector<Index> && rhs);// = delete;
  Vector<Index>& operator=(const Vector<Index>&);// = delete;
  Vector<Index>& operator=(Vector<Index> && rhs);// = delete;

  std::string getInfo() const;

  std::string getDebugInfo() const;

    value_type                                                _maxIndex;
  mutable
    std::unique_ptr<detail::Distribution<Vector<Index>>>      _distribution;
};

} // namespace skelcl

#include "detail/IndexVectorDef.h"

#endif // INDEX_VECTOR_H_

