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
///  IndexMatrix.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef INDEX_MATRIX_H_
#define INDEX_MATRIX_H_

#include <vector>

#include "Index.h"
#include "Matrix.h"
#include "Vector.h"

#include "detail/DeviceList.h"
#include "detail/skelclDll.h"

namespace skelcl {

///
/// \brief An simple to use alias for the Matrix<IndexPoint> class.
///
typedef const Matrix<IndexPoint> IndexMatrix;

/// 
/// \brief The IndexMatrix (a.k.a. Matrix<IndexPoint>) class is a special
///        implementation of a Matrix with Elements of type IndexPoint.
///
/// The implementation guaranties that no data is transfered to and from the
/// devices when using this version of the Matrix. OpenCL functionality (global
/// ids) is used to make the IndexPoint%s available on the device. This
/// implementation requires, that the Matrix is const and can, therefore, not be
/// modified after creation.
///
/// \ingroup containers
/// \ingroup matrix
/// 
template <>
class SKELCL_DLL Matrix<IndexPoint> {
public:
  ///
  /// \brief Defines the type of the elements in the Matrix
  ///
  typedef IndexPoint value_type;

  ///
  /// \brief Defines the type used to denote size of the Matrix
  ///
  typedef skelcl::MatrixSize size_type;

  ///
  /// \brief Create a new IndexMatrix with the given size.
  ///
  Matrix(const size_type size);

  ~Matrix();

  // TODO: finish matrix interface
//  typedef ?? const_iterator;
//  const_iterator begin() const;
//  const_iterator end() const;
  //  ...

  ///
  /// \brief Returns the total size of the Matrix.
  ///
  /// \return The total size of the Matrix.
  ///
  size_type size() const;

  ///
  /// \brief Returns the size of the parts of the Matrix which are stored on
  ///        each device.
  ///
  /// \return The sizes of the parts of the Matrix which are stored on each
  ///         device.
  ///
  detail::Sizes sizes() const;

  ///
  /// \brief Returns the IndexPoint at position n (i.e. n).
  ///
  /// This version does not prevent out of range accesses.
  /// If n > size() the behavior is undefined.
  ///
  /// \param n The position for which the IndexPoint should be returned.
  ///
  /// \return The IndexPoint at position n. As the Matirx stores IndexPoint%s
  ///         from 0 up to its size the return value is always n.
  ///
  value_type operator[]( size_type n ) const;

  /// \brief Returns the IndexPoint at position n (i.e. n).
  ///
  /// This version does prevent out of range accesses.
  /// If n > size() an std::out_of_range exception is thrown.
  ///
  /// \param n The position for which the IndexPoint should be returned.
  ///
  /// \return The IndexPoint at position n. As the Matirx stores IndexPoint%s
  ///         from 0 up to its size the return value is always n.
  ///
  value_type at( size_type n ) const;

  ///
  /// \brief Return the first IndexPoint stored in the Matrix (i.e. {0, 0}).
  ///
  /// \return Always {0, 0}.
  ///
  value_type front() const;

  ///
  /// \brief Return the last Index stored in the Vector (i.e.
  ///        {size().rowCount()-1, size().columnCount()-1}).
  ///
  /// \return Always {size().rowCount()-1, size().columnCount()-1}).
  ///
  value_type back() const;

  ///
  /// \brief Returns the current distribution of the Matrix.
  ///
  /// \return The current distribution of the Matrix.
  ///
  detail::Distribution<Matrix<IndexPoint>>& distribution() const;

  ///
  /// \brief Changes the distribution of the Matrix
  ///
  /// \param distribution The new distribution to be set. After this call
  ///                     distribution is the new selected distribution of the
  ///                     Matrix.
  ///
  template <typename U>
  void setDistribution(const detail::Distribution<Matrix<U>>& distribution)
      const;

  ///
  /// \brief Changes the distribution of the Matrix
  ///
  /// \param newDistribution The new distribution to be set. After this call
  ///                     distribution is the new selected distribution of the
  ///                     Matrix.
  ///
  template <typename U>
  void setDistribution(const std::unique_ptr<detail::Distribution<Matrix<U>>>&
                          newDistribution) const;

  ///
  /// \brief Changes the distribution of the Matrix
  ///
  /// \param newDistribution The new distribution to be set. After this call
  ///                     distribution is the new selected distribution of the
  ///                     Matrix.
  ///
  void setDistribution(std::unique_ptr<
      detail::Distribution<Matrix<IndexPoint>>>&& newDistribution) const;

  ///
  /// \brief Returns a string defining functions necessary to access the Matrix
  ///        on the device. For the IndexMatrix this function always returns an
  ///        empty string.
  ///
  /// This functions exists for compatibility reasons with the Matrix class.
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
  std::vector<IndexPoint>& hostBuffer() const;

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
  Matrix();// = delete;
  Matrix(const Matrix<IndexPoint>& rhs);// = delete;
  Matrix(Matrix<IndexPoint> && rhs);// = delete;
  Matrix<IndexPoint>& operator=(const Matrix<IndexPoint>&);// = delete;
  Matrix<IndexPoint>& operator=(Matrix<IndexPoint> && rhs);// = delete;

  std::string getInfo() const;

  std::string getDebugInfo() const;

  value_type                                                  _maxIndex;
  mutable
    std::unique_ptr<detail::Distribution<Matrix<IndexPoint>>> _distribution;
};

} // namespace skelcl

#include "detail/IndexMatrixDef.h"

#endif // INDEX_MATRIX_H_

