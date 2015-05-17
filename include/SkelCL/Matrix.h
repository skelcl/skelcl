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
///  Matrix.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
/// \author Matthias Buss
///

#ifndef MATRIX_H_
#define MATRIX_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include "Distributions.h"

#include "detail/Device.h"
#include "detail/DeviceBuffer.h"
#include "detail/Distribution.h"
#include "detail/Padding.h"
#include "detail/skelclDll.h"

namespace skelcl {

///
/// \brief This class defines a two dimensional size for a Matrix.
///
/// The number of rows and the number of columns are stored.
/// Objects of this type can be compared against each other.
///
class SKELCL_DLL MatrixSize {
public:
  typedef size_t size_type;

  ///
  /// \brief Create a new MatrixSize object with the given number of rows and
  ///        number of columns.
  ///
  /// \param rowCount The number of rows.
  /// \param columnCount The number of columns.
  ///
  MatrixSize(size_type rowCount, size_type columnCount);

  ///
  /// \brief Returns the total number of elements.
  ///        I.e. rowCount() * columnCount().
  ///
  /// \return rowCount() * columnCount()
  ///
  size_type elemCount() const;

  ///
  /// \brief Returns the number of rows.
  ///
  /// \return The number of rows.
  ///
  size_type rowCount() const;

  ///
  /// \brief Returns the number of columns.
  ///
  /// \return The number of columns.
  ///
  size_type columnCount() const;

  ///
  /// \brief Compares two MatrixSizes for equality.
  ///
  /// \param rhs The MatrixSize to compare with.
  ///
  /// \return Returns true if and only if rowCount() is equal for this and rhs
  ///         and columnCount() is equal for this and rhs.
  ///
  bool operator==(const MatrixSize& rhs) const;

  ///
  /// \brief Compares two MatrixSizes for inequality.
  ///
  /// \param rhs The MatrixSize to compare with.
  ///
  /// \return Returns !(this == rhs)
  ///
  bool operator!=(const MatrixSize& rhs) const;

  ///
  /// \brief Compares if this is greater than rhs.
  ///
  /// \param rhs The MatrixSize to compare with.
  ///
  /// \return Returns true if columnCount() is equal for this and rhs and
  ///         rowCount() for this is greater than for rhs.
  ///         Returns also true if columnCount() for this is greater than
  ///         for rhs and rowCount() for this is greater or equal than
  ///         for rhs.
  ///         Returns false otherwise.
  ///
  bool operator> (const MatrixSize& rhs) const;

  ///
  /// \brief Compares if this is less than rhs.
  ///
  /// \param rhs The MatrixSize to compare with.
  ///
  /// \return Returns true if columnCount() is equal for this and rhs and
  ///         rowCount() for this is less than for rhs.
  ///         Returns also true if columnCount() for this is less than for
  ///         rhs and rowCount() for this is less or equal than for rhs.
  ///         Return false otherwise.
  ///
  bool operator< (const MatrixSize& rhs) const;

  ///
  /// \brief Compares if this is greater than rhs.
  ///
  /// \param rhs The MatrixSize to compare with.
  ///
  /// \return Returns true if columnCount() is equal for this and rhs and
  ///         rowCount() for this is greater or equal than for rhs.
  ///         Returns also true if columnCount() for this is greater than
  ///         for rhs and rowCount() for this is greater or equal than
  ///         for rhs.
  ///         Returns false otherwise.
  ///
  bool operator>=(const MatrixSize& rhs) const;

  ///
  /// \brief Compares if this is less than rhs.
  ///
  /// \param rhs The MatrixSize to compare with.
  ///
  /// \return Returns true if columnCount() is equal for this and rhs and
  ///         rowCount() for this is less or equal than for rhs.
  ///         Returns also true if columnCount() for this is less than for
  ///         rhs and rowCount() for this is less or equal than for rhs.
  ///         Return false otherwise.
  ///
  bool operator<=(const MatrixSize& rhs) const;

private:
  size_type _rowCount;
  size_type _columnCount;
};

/// \cond
/// Don't show detail namespace in doxygen
/// TODO: move into detail namespace?
template <typename T>
class RegisterMatrixDeviceFunctions {
public:
  RegisterMatrixDeviceFunctions();
};
/// \endcond

///
/// \defgroup matrix Matrix
/// \brief Two dimensional container data structures
///
/// \ingroup containers
///

///
/// \brief The Matrix class is a two dimensional container which makes its data
///        accessible on the host as well as on the devices.
///
/// \ingroup containers
/// \ingroup matrix
///
template <typename T>
class Matrix {
public:
  typedef std::vector<T> host_buffer_type;
  typedef typename host_buffer_type::value_type value_type;
  typedef typename host_buffer_type::pointer pointer;
  typedef typename host_buffer_type::const_pointer const_pointer;
  typedef typename host_buffer_type::reference reference;
  typedef typename host_buffer_type::const_reference const_reference;
  typedef typename host_buffer_type::iterator iterator;
  typedef typename host_buffer_type::const_iterator const_iterator;
  typedef typename skelcl::MatrixSize size_type;
  typedef typename host_buffer_type::difference_type difference_type;
  typedef typename host_buffer_type::allocator_type allocator_type;

  ///
  /// \brief This struct represents two dimensional coordinates.
  ///
  struct coordinate {
    typedef size_type::size_type index_type;

    index_type rowIndex;
    index_type columnIndex;
  };

  ///
  /// \brief Creates an empty new Matrix of size {0,0}.
  ///
  Matrix();

  ///
  /// \brief Creates a new Matrix with the given MatrixSize and Distribution.
  ///        The newly created Matrix is filled with the given value.
  ///
  /// \param size The size of the Matrix to create.
  /// \param value The value to be used to fill the Matrix.
  /// \param distribution The distribution to be used by the Matrix.
  ///
  ///
  Matrix(const size_type size,
         const value_type& value = value_type(),
         const detail::Distribution<Matrix<T>>& distribution
                                    = detail::Distribution<Matrix<T>>());

  ///
  /// \brief Constructor with a std::vector and the number of columns as
  ///  parameter
  ///
  Matrix(const std::vector<T>& vector,
         const typename size_type::size_type columnCount,
         const detail::Distribution<Matrix<T>>& distribution
            = detail::Distribution<Matrix<T>>());

  ///
  /// \brief constructor with a std::vector and a matrix_size as parameter
  ///
  Matrix(const std::vector<T>& vector,
         const size_type size,
         const detail::Distribution<Matrix<T>>& distribution
            = detail::Distribution<Matrix<T>>());

  ///
  /// \brief static function creating a matrix from 2 dim std::vector as
  ///        parameter
  ///
  static Matrix<T> from2DVector(const std::vector<std::vector<T>>& matrix,
                                const detail::Distribution<Matrix<T>>&
                                    distribution
                                      = detail::Distribution<Matrix<T>>());

  ///
  /// \brief constructor with 2 iterators and the number of columns as parameter
  ///
  template <class InputIterator>
  Matrix(InputIterator first, InputIterator last,
         const typename size_type::size_type columnCount,
         const detail::Distribution<Matrix<T>>& distribution
             = detail::Distribution<Matrix<T>>());

   ///
   /// \brief constructor with 2 iterators and a matrix_size as parameter
   ///
  template <class InputIterator>
    Matrix(InputIterator first, InputIterator last,
           const size_type size,
           const detail::Distribution<Matrix<T>>& distribution
              = detail::Distribution<Matrix<T>>());

  ///
  /// \brief   Move constructor
  ///
  Matrix(Matrix<T>&& rhs);

  ///
  /// \brief  Move assignment operator
  ///
  Matrix<T>& operator=(Matrix<T>&& rhs);

  ///
  /// \brief Destructor
  ///
  ~Matrix();

  // matrix interface

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  iterator row_begin(typename coordinate::index_type rowIndex);
  const_iterator row_begin(typename coordinate::index_type rowIndex) const;

  iterator row_end(typename coordinate::index_type rowIndex);
  const_iterator row_end(typename coordinate::index_type rowIndex) const;

  iterator operator[]( typename coordinate::index_type rowIndex );

  const_iterator operator[]( typename coordinate::index_type rowIndex ) const;

  size_type size() const;
  typename size_type::size_type rowCount() const;
  typename size_type::size_type columnCount() const;

//  detail::Sizes sizes() const;

  size_type::size_type max_size() const;
  void resize(const size_type& size, T c = T());
  size_type::size_type capacity() const;
  bool empty() const;
  void reserve(size_type::size_type bytes);

  // Element access
  reference operator()( coordinate c );
  const_reference operator()( coordinate c ) const;

  reference at( coordinate c);
  const_reference at( coordinate c) const;

  reference front();
  const_reference front() const;

  reference back();
  const_reference back() const;

  // Modifiers
  template <class InputIterator>
  void assign(InputIterator first, InputIterator last);

  void assign(size_type size, const T& v);

  template <class InputIterator>
  void push_back_row(InputIterator first, InputIterator last);

  void push_back_row(std::initializer_list<T> list);
  void pop_back_row();

  template <class InputIterator>
  void insert_row(typename coordinate::index_type rowIndex,
                  InputIterator first, InputIterator last);
  void insert_row(typename coordinate::index_type rowIndex,
                  std::initializer_list<T> list);

  iterator erase_row(typename coordinate::index_type rowIndex);

  void clear();

  // Some functions from parent Container
  detail::Distribution<Matrix<T>>& distribution() const;

  template <typename U>
  void setDistribution(const detail::Distribution<Matrix<U>>&
                          distribution) const;

  template <typename U>
  void setDistribution(const std::unique_ptr<detail::Distribution<Matrix<U>>>&
                          newDistribution) const;

  void setDistribution(std::unique_ptr<detail::Distribution<Matrix<T>>>&&
                          newDistribution) const;

  void createDeviceBuffers() const;

  void forceCreateDeviceBuffers() const;

  detail::Event startUpload() const;

  void copyDataToDevices() const;

  detail::Event startDownload() const;

  void copyDataToHost() const;

  void dataOnDeviceModified() const;

  void dataOnHostModified() const;

  const detail::DeviceBuffer& deviceBuffer(const detail::Device& device)const;

  host_buffer_type& hostBuffer() const;

  static std::string deviceFunctions();

private:
  ///
  /// \brief  Explicit deleted copy constructor
  ///
  Matrix(const Matrix<T>&);// = delete;

  ///
  /// \brief  Explicit deleted assignment operator
  ///
  Matrix<T>& operator=(const Matrix<T>&);// = delete;

  std::string getInfo() const;
  std::string getDebugInfo() const;


  static RegisterMatrixDeviceFunctions<T> registerMatrixDeviceFunctions;

          MatrixSize                                  _size;
  mutable
    std::unique_ptr<detail::Distribution<Matrix<T>>>  _distribution;
  mutable bool                                        _hostBufferUpToDate;
  mutable bool                                        _deviceBuffersUpToDate;
  mutable host_buffer_type                            _hostBuffer;
    // _deviceBuffers empty => buffers not created
  mutable std::map< detail::Device::id_type,
                    detail::DeviceBuffer >            _deviceBuffers;

  // Profiling information.
  mutable std::vector<cl::Event> _uevents;
  mutable std::vector<cl::Event> _devents;
  void printEventTimings(std::vector<cl::Event> events,
                         const std::string direction) const;
};

template <typename T>
RegisterMatrixDeviceFunctions<T> Matrix<T>::registerMatrixDeviceFunctions;

} // namespace skelcl

#include "detail/MatrixDef.h"

#endif // MATRIX_H_
