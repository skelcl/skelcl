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
///  Vector.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef VECTOR_H_
#define VECTOR_H_

#include <memory>
#include <string>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include "detail/CopyDistribution.h"
#include "detail/Device.h"
#include "detail/DeviceBuffer.h"
#include "detail/Distribution.h"

namespace skelcl {

namespace detail {

  class Event;

  class Sizes {
  public:
    void push_back(detail::DeviceBuffer::size_type size) {
      _sizes.push_back(size);
    }

    cl_uint operator[](size_t index) const {
      return static_cast<cl_uint>(_sizes[index]);
    }
  private:
    std::vector<detail::DeviceBuffer::size_type> _sizes;
  };

} // namespace detail

template <typename T>
class Vector {
  typedef std::vector<T> vector_type;
  typedef std::shared_ptr<std::vector<T> > vector_shared_pointer;
public:
  typedef std::vector<T> host_buffer_type;
  typedef typename host_buffer_type::value_type value_type;
  typedef typename host_buffer_type::pointer pointer;
  typedef typename host_buffer_type::const_pointer const_pointer;
  typedef typename host_buffer_type::reference reference;
  typedef typename host_buffer_type::const_reference const_reference;
  typedef typename host_buffer_type::iterator iterator;
  typedef typename host_buffer_type::const_iterator const_iterator;
  typedef typename host_buffer_type::const_reverse_iterator const_reverse_iterator;
  typedef typename host_buffer_type::reverse_iterator reverse_iterator;
  typedef typename host_buffer_type::size_type size_type;
  typedef typename host_buffer_type::difference_type difference_type;
  typedef typename host_buffer_type::allocator_type allocator_type;

  ///
  /// \brief same semantics as std::vector
  ///
  Vector();

  ///
  /// \brief same semantics as std::vector
  ///
  Vector(const size_type size, const value_type& value = value_type());

  ///
  /// \brief Constructs a new vector with size elements
  ///
  /// \param size         The vector is constructed with size number of elements
  ///        distribution This is used as distribution for the new constructed
  ///                     vector
  ///        value        This value is used to initialize the elements of the
  ///                     new constructed vector
  ///
  Vector(const size_type size,
         const detail::Distribution< Vector<T> >& distribution,
         const value_type& value = value_type());

  ///
  /// \brief same semantics as std::vector
  ///
  template <class InputIterator>
  Vector(InputIterator first, InputIterator last);

  ///
  /// \brief Constructs a new vector by copying the elements between first and
  ///        last
  ///
  /// \param first        Input iterator pointing to the first element of the
  ///                     range [first, last) which is used for initialization
  ///                     of the elements of the new constructed vector
  ///        last         Input iterator pointing to the last element of the
  ///                     range [first, last) which is used for initialization
  ///                     of the elements of the new constructed vector
  ///        distribution This is used as distribution for the new constructed
  ///                     vector
  ///
  template <class InputIterator>
  Vector(InputIterator first,
         InputIterator last,
         const detail::Distribution< Vector<T> >& distribution);

  ///
  /// \brief Copy construction
  ///
  Vector(const Vector<T>& rhs);

  ///
  /// \brief Move constructor
  ///
  Vector(Vector<T>&& rhs);

  ///
  /// \brief Copy assignment operator
  ///
  Vector<T>& operator=(const Vector<T>&);

  ///
  /// \brief Move assignment operator
  ///
  Vector<T>& operator=(Vector<T>&& rhs);

  ///
  /// \brief same semantics as std::vector
  ///
  ~Vector();

  // vector interface

  ///
  /// \brief same semantics as std::vector
  ///
  iterator begin();

  ///
  /// \brief same semantics as std::vector
  ///
  const_iterator begin() const;

  ///
  /// \brief same semantics as std::vector
  ///
  iterator end();

  ///
  /// \brief same semantics as std::vector
  ///
  const_iterator end() const;

#if 0
  reverse_iterator rbegin();

  const_reverse_iterator rbegin() const;

  reverse_iterator rend();

  const_reverse_iterator rend() const;
#endif

  ///
  /// \brief same semantics as std::vector
  ///
  size_type size() const;

  ///
  /// \brief Returns the number of elements stored on each device
  ///
  /// \return Returns a sizes object describing the number of elements stored
  ///         on each device
  ///
  detail::Sizes sizes() const;

  ///
  /// \brief same semantics as std::vector
  ///
  size_type max_size() const;

  ///
  /// \brief same semantics as std::vector
  ///
  void resize( size_type sz, T c = T() );

  ///
  /// \brief same semantics as std::vector
  ///
  size_type capacity() const;

  ///
  /// \brief same semantics as std::vector
  ///
  bool empty() const;

  ///
  /// \brief same semantics as std::vector
  ///
  void reserve( size_type n );

  ///
  /// \brief same semantics as std::vector
  ///
  reference operator[]( size_type n );

  ///
  /// \brief same semantics as std::vector
  ///
  const_reference operator[]( size_type n ) const;

  ///
  /// \brief same semantics as std::vector
  ///
  reference at( size_type n );

  ///
  /// \brief same semantics as std::vector
  ///
  const_reference at( size_type n ) const;

  ///
  /// \brief same semantics as std::vector
  ///
  reference front();

  ///
  /// \brief same semantics as std::vector
  ///
  const_reference front() const;

  ///
  /// \brief same semantics as std::vector
  ///
  reference back();

  ///
  /// \brief same semantics as std::vector
  ///
  const_reference back() const;

  ///
  /// \brief same semantics as std::vector
  ///
  template <class InputIterator>
  void assign( InputIterator first, InputIterator last );

  ///
  /// \brief same semantics as std::vector
  ///
  void assign( size_type n, const T& u );

  ///
  /// \brief same semantics as std::vector
  ///
  void push_back( const T& x );

  ///
  /// \brief same semantics as std::vector
  ///
  void pop_back();

  ///
  /// \brief same semantics as std::vector
  ///
  iterator insert( iterator position, const T& x );

  ///
  /// \brief same semantics as std::vector
  ///
  void insert( iterator position, size_type n, const T& x );

  ///
  /// \brief same semantics as std::vector
  ///
  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last);

  ///
  /// \brief same semantics as std::vector
  ///
  iterator erase( iterator position );

  ///
  /// \brief same semantics as std::vector
  ///
  iterator erase( iterator first, iterator last );

  // TODO: should probably not be implemented
  void swap( Vector<T>& rhs );

  ///
  /// \brief same semantics as std::vector
  ///
  void clear();

  // TODO: should probably not be implemented
  allocator_type get_allocator() const;

  ///
  /// \brief Returns a pointer to the current distribution of the vector.  ///
  /// \return A pointer to the current distribution of the vector, of nullptr
  ///         if no distribution is set
  ///
  detail::Distribution< Vector<T> >& distribution() const;

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
  void setDistribution(const detail::Distribution< Vector<U> >& distribution) const;

  ///
  /// \brief Create buffers on the devices involved in the current distribution
  ///
  /// This function is a no-op if the buffers are already created. If you want
  /// to force the creation, e.g. replace existing buffers, use
  /// forceCreateDeviceBuffers
  ///
  void createDeviceBuffers() const;

  ///
  /// \brief Forces the creation of buffers on the devices involved in the
  ///        current distribution, even if this means replacing existing buffers
  ///
  /// If you want to create the buffers only if no buffers are already created
  /// use createDeviceBuffers
  ///
  void forceCreateDeviceBuffers() const;

  ///
  /// \brief Starts copying data from the host to the devices involved in the
  ///        current distribution.
  ///
  /// This function returns immediately and does not wait until the copy
  /// operation is finished. The event object returned can be used to wait
  /// explicitly for the copy operation to complete. For an blocking version use
  /// copyDataToDevices
  ///
  /// \return An event object which can be used to explicitly wait for the copy
  ///         operation to complete
  ///
  detail::Event startUpload() const;

  ///
  /// \brief Copies data from the host to the devices involved in the current
  ///        distribution.
  ///
  /// This function blocks until the copy operation is finished. For an
  /// unblocking version use startUpload.
  ///
  void copyDataToDevices() const;

  ///
  /// \brief Starts copying data from the devices involved in the current
  ///        distribution to the host
  ///
  /// This function returns immediately and does not wait until the copy
  /// operation is finished. The event object returned can be used to wait
  /// explicitly for the copy operation to complete. For an blocking version use
  /// copyDataToHost
  ///
  /// \return An event object which can be used to explicitly wait for the copy
  ///         operation to complete
  ///
  detail::Event startDownload() const;

  ///
  /// \brief Copies data from the devices involved in the current distribution
  ///        to the host
  ///
  /// This function blocks until the copy operation is finished. For an
  /// unblocking version use startDownload.
  ///
  void copyDataToHost() const;

  ///
  /// \brief Marks the data on the device as been modified
  ///
  void dataOnDeviceModified() const;

  ///
  /// \brief Marks the data on the host as been modified
  ///
  void dataOnHostModified() const;

  ///
  /// \brief Returns the buffer for the given device used to store elements of
  ///        the vector accordingly to the current distribution.
  ///
  /// \param device The device for which the buffer should be returned.
  ///               The device must be part of the current distribution and the
  ///               device buffers have to be already created, otherwise the
  ///               behavior is undefined.
  ///
  /// \return A reference to the buffer object used for the given device.
  ///         Be careful if you use auto to use auto& to capture the reference
  ///         and not making an implicit copy by using plain auto.
  ///
  const detail::DeviceBuffer& deviceBuffer(const detail::Device& device) const;

  host_buffer_type& hostBuffer() const;

private:
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

          size_type                                       _size;
  mutable
    std::unique_ptr< detail::Distribution< Vector<T> > >  _distribution;
  mutable bool                                            _hostBufferUpToDate;
  mutable bool                                            _deviceBuffersUpToDate;
  mutable host_buffer_type                                _hostBuffer;
  // _deviceBuffers empty => buffers not created yet
  mutable std::vector<detail::DeviceBuffer>               _deviceBuffers;
};

} // namespace skelcl

#include "detail/VectorDef.h"

#endif // VECTOR_H_
