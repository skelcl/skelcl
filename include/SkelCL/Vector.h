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

#include <map>
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

/// \cond
/// Don't show detail namespace in doxygen
namespace detail {

  class Event;

  class Sizes {
  public:
    Sizes() : _sizes() {}
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
/// \endcond


/// \cond
/// Don't show detail namespace in doxygen
/// TODO: move into detail namespace?
template <typename T>
class RegisterVectorDeviceFunctions {
public:
  RegisterVectorDeviceFunctions();
};
/// \endcond

/// \defgroup vector Vector
/// \brief One dimensional container data structures
///
/// \ingroup containers
///

/// \brief The Vector class is a one dimensional container which makes its data
///        accessible on the host as well as on the devices.
///
/// The interface resembles the interface of the std::vector.
/// Access to elements is possible by calling member functions or using
/// iterators.
///
/// Different to the std::vector are the possibility to set a Distribution,
/// which explains how the elements should be distributed across multiple
/// devices. In addition, there exist functions to copy the element to and from
/// the devices and to access the underlying OpenCL objects.
///
/// \ingroup containers
/// \ingroup vector
///
template <typename T>
class Vector {
public:
  /// \brief The type used to store the elements on the host
  ///
  typedef std::vector<T> host_buffer_type;
  /// \brief The type of the elements
  ///
  typedef typename host_buffer_type::value_type value_type;
  /// \brief The type of a pointer to an element
  ///
  typedef typename host_buffer_type::pointer pointer;
  /// \brief The type of a const pointer to an element
  ///
  typedef typename host_buffer_type::const_pointer const_pointer;
  /// \brief The type of a reference to an element
  ///
  typedef typename host_buffer_type::reference reference;
  /// \brief The type of a const reference to an element
  ///
  typedef typename host_buffer_type::const_reference const_reference;
  /// \brief The type of an iterator
  ///
  typedef typename host_buffer_type::iterator iterator;
  /// \brief The type of an const iterator
  ///
  typedef typename host_buffer_type::const_iterator const_iterator;
  /// \brief The type of a reverse iterator
  ///
  typedef typename host_buffer_type::reverse_iterator reverse_iterator;
  /// \brief The type of a const reverse iterator
  ///
  typedef typename host_buffer_type::const_reverse_iterator
          const_reverse_iterator;
  /// \brief The integral type used to define the number of the elements in the
  ///        Vector
  typedef typename host_buffer_type::size_type size_type;
  /// \brief The integral type used to define differences of two size_type
  ///        values
  typedef typename host_buffer_type::difference_type difference_type;
  /// \brief The type of the allocator used on the host
  ///
  typedef typename host_buffer_type::allocator_type allocator_type;

  /// \brief Creates a new empty Vector.
  ///
  Vector();

  /// \brief Creates a new Vector with size many elements.
  ///
  /// \b Complexity Constant
  ///
  /// \param size         The number of elements of the newly created Vector
  /// \param value        This value is used to initialize the elements of the
  ///                     new constructed Vector
  /// \param distribution Distribution to be used by the new constructed
  ///                     Vector
  Vector(const size_type size,
         const value_type& value = value_type(),
         const detail::Distribution<Vector<T>>& distribution
                                    = detail::Distribution<Vector<T>>());

  /// \brief Creates a new Vector with the content of the range
  ///          <tt>[first, last)</tt>.
  ///
  /// \b Complexity Linear in distance between \c first and \c last
  /// 
  /// \param first Begin of the range to copy the elements from
  /// \param last  End of the range to copy the elements from
  template <class InputIterator>
  Vector(InputIterator first, InputIterator last);
  
  /// \brief Creates a new Vector with the content of the range
  ///          <tt>[first, last)</tt>.
  ///
  /// \b Complexity Linear in distance between \c first and \c last
  /// \param first Begin of the range to copy the elements from
  /// \param last  End of the range to copy the elements from
  /// \param distribution Distribution to be used by the new constructed
  ///                     Vector
  template <class InputIterator>
  Vector(InputIterator first,
         InputIterator last,
         const detail::Distribution<Vector<T>>& distribution);

  /// \brief Copy constructor. Creates a new Vector with the copy of the content
  ///        of \c rhs.
  ///
  /// \b Complexity Linear in size of \c rhs
  /// \param rhs Another Vector to be used as source to initialize the elements
  ///            of the Vector with
  Vector(const Vector<T>& rhs);

  /// \brief Move constructor. Creates a new Vector by moving the content of
  ///        \c rhs.
  ///
  /// \b Complexity Constant
  /// \param rhs Another Vector to be used as source to initialize the elements
  ///        of the Vector with
  Vector(Vector<T>&& rhs);

  ///
  /// \brief Copy assignment operator. Replaces the content with a copy of the
  ///        content of \c rhs.
  ///
  /// \b Complexity Linear in size pf \c rhs
  /// \param rhs Another Vector to be used as source to initialize the elements
  ///            of the Vector with
  Vector<T>& operator=(const Vector<T>& rhs);

  ///
  /// \brief Move assignment operator. Replaces the content by moving the
  ///        content of \c rhs.
  ///
  /// \b Complexity Constant
  /// \param rhs Another Vector to be used as source to initialize the elements
  ///            of the Vector with
  Vector<T>& operator=(Vector<T>&& rhs);

  /// \brief Destructs the Vector.
  ///
  /// \b Complexity Linear in the size of the Vector
  ~Vector();

  /// \brief Returns an iterator to the first element of the Vector.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// If the container is empty, the returned iterator will be equal to end().
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  ///
  /// \return Iterator to the first element
  iterator begin();

  /// \brief Returns a constant iterator to the first element of the Vector.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// If the container is empty, the returned iterator will be equal to end().
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  ///
  /// \return Constant iterator to the first element
  const_iterator begin() const;

  /// \brief Returns an iterator to the element following the last element of
  ///        the Vector.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  ///
  /// \return Iterator to the element following the last element
  iterator end();

  /// \brief Returns a constant iterator to the element following the last
  ///        element of the Vector.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  ///
  /// \return Constant iterator to the element following the last element
  const_iterator end() const;

#if 0
  reverse_iterator rbegin();

  const_reverse_iterator rbegin() const;

  reverse_iterator rend();

  const_reverse_iterator rend() const;
#endif

  /// \brief Returns the number of elements in the Vector.
  ///
  /// \b Complexity Constant
  ///
  /// \return The number of elements in the Vector
  size_type size() const;

  /// \brief Returns the number of elements stored on each device
  ///
  /// \b Complexity Linear in number of devices (usually small)
  ///
  /// \return Returns a sizes object describing the number of elements stored
  ///         on each device
  detail::Sizes sizes() const;

  /// \brief Return the maximum number of elements the Vector is able to hold.
  ///
  /// \b Complexity Constant
  /// \return Maximum number of elements.
  size_type max_size() const;

  /// \brief Resizes the container to contain \c count elements.
  ///
  /// If the current size is greater than \c count, the Vector is reduced to its
  /// first \c count elements as if by repeatedly calling pop_back()
  ///
  /// \b Complexity Linear in the size of the Vector
  /// \param count New size of the Vector
  /// \param value The value to initialize the new elements with
  void resize( size_type count, T value = T() );

  /// \brief Returns the number of elements that the Vector has currently
  ///        allocated space for.
  ///
  /// \b Complexity Constant
  /// \return Capacity of the currently allocated storage.
  size_type capacity() const;

  /// \brief Checks if the Vector has no elements, i.e. whether
  ///        <tt>begin() == end()</tt>
  ///
  /// \b Complexity Constant
  /// \return \c true if the container is empty, \c false otherwise
  bool empty() const;

  /// \brief Increases the capacity of the Vector to a value that's greater or
  ///        equal to \c n.
  ///
  /// If \c n is greater than current capacity(), new storage is allocated,
  /// otherwise the function does nothing.
  /// If \c is greater than capacity(), all iterators and references are
  /// invalidated. Otherwise, no iterators or references are invalidated.
  ///
  /// \b Complexity At most linear in the size() of the Vector
  /// \param n New capacity of the Vector
  void reserve( size_type n );

  /// \brief Returns a reference to the element at the specified location
  ///        \c pos. No boundary checks are performed.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  /// \param pos Position of the element to access
  /// \return Reference to the requested element
  reference operator[]( size_type pos );

  /// \brief Returns a constant reference to the element at the specified
  ///        location \c pos. No boundary checks are performed.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  /// \param pos Position of the element to access
  /// \return Constant reference to the requested element
  const_reference operator[]( size_type pos ) const;

  /// \brief Returns a reference to the element at the specified location
  ///        \c pos. Boundary checks are performed.
  ///
  /// If \c pos is not within the range of the Vector, an exception of the type
  /// std::out_of_range is thrown.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  /// \param pos Position of the element to access
  /// \return Reference to the requested element
  reference at( size_type pos );

  /// \brief Returns a constant reference to the element at the specified
  ///        location \c pos. Boundary checks are performed.
  ///
  /// If \c pos is not within the range of the Vector, an exception of the type
  /// std::out_of_range is thrown.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  /// \param pos Position of the element to access
  /// \return Constant reference to the requested element
  const_reference at( size_type pos ) const;

  /// \brief Returns a reference to the first element in the Vector.
  ///        Calling front() on an empty Vector is undefined.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  /// \return Reference to the first element
  reference front();

  /// \brief Returns a constant reference to the first element in the Vector.
  ///        Calling front() on an empty Vector is undefined.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  /// \return Constant reference to the first element
  const_reference front() const;

  /// \brief Returns a reference to the last element in the Vector.
  ///        Calling back() on an empty Vector is undefined.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  /// \return Reference to the last element
  reference back();

  /// \brief Returns a constant reference to the last element in the Vector.
  ///        Calling back() on an empty Vector is undefined.
  ///
  /// If the data on the host is not up to date this function will block until
  /// the elements of the Vector are transfered from the devices to the host.
  ///
  /// \b Complexity Constant if hostIsUpToDate() returns \c true.
  ///               Linear in size of the Vector otherwise.
  /// \return Constant reference to the last element
  const_reference back() const;

  /// \brief Replaces the contents of the Vector with copies of the elements in
  ///        the range <tt>[first, last)</tt>.
  ///
  /// \b Complexity Linear in distance between \c first and \c last
  /// \param first Begin of the range to copy the elements from
  /// \param last  End of the range to copy the elements from
  template <class InputIterator>
  void assign( InputIterator first, InputIterator last );

  /// \brief Replaces the contents of the Vector with \c count copies of
  ///        \c value.
  ///
  /// \b Complexity Linear in \c count
  /// \param count The new size of the Vector
  /// \param value  The value to initialize the elements of the Vector with
  void assign( size_type count, const T& value );

  /// \brief Appends a copy of the given element value to the end of the Vector.
  ///
  /// \b Complexity Amortized constant.
  /// \param value The value of the element to append
  void push_back( const T& value );

  /// \brief Removes the last element of the Vector.
  ///
  /// No iterators or references except for back() and end() are invalidated.
  ///
  /// \b Complexity Constant
  void pop_back();

  /// \brief Inserts copy of \c value at the specified location in the Vector.
  ///
  /// \b Complexity Constant plus linear in the distance between \c pos and end
  ///               of the Vector
  /// \param pos Iterator to the location before which the copy of \c value will
  ///            be inserted. \c pos may be the end() iterator.
  /// \param value Element value to insert
  /// \return Iterator pointing to the inserted value
  iterator insert( iterator pos, const T& value );

  ///
  /// \brief same semantics as std::vector
  ///
  void insert( iterator pos, size_type n, const T& value );

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
  /// \brief Returns a pointer to the current distribution of the vector.
  ///
  /// \return A pointer to the current distribution of the vector, of nullptr
  ///         if no distribution is set
  ///
  detail::Distribution<Vector<T>>& distribution() const;

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

  void setDistribution(std::unique_ptr<detail::Distribution<Vector<T>>>&&
                          newDistribution) const;

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

  detail::DeviceBuffer& deviceBuffer(const detail::Device& device);

  host_buffer_type& hostBuffer() const;

  static std::string deviceFunctions();

  /// \brief Returns if the elements stored on the host are up to date, or if
  ///        the elements are outdated because the elements on the devices have
  ///        been modified more recently.
  ///
  /// \b Complexity Constant
  /// \return Returns true if and only if the most recent modifications to the
  ///         elements are available on the host.
  bool hostIsUpToDate() const;

  /// \brief Returns if the elements stored on the devices are up to date, or if
  ///        the elements are outdated because the elements on the host have
  ///        been modified more recently.
  ///
  /// \b Complexity Constant
  /// \return Returns true if and only if the most recent modifications to the
  ///         elements are available on the devices.
  bool devicesAreUpToDate() const;

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

  static RegisterVectorDeviceFunctions<T> registerVectorDeviceFunctions;

          size_type                                   _size;
  mutable
    std::unique_ptr<detail::Distribution<Vector<T>>>  _distribution;
  mutable bool                                        _hostBufferUpToDate;
  mutable bool                                        _deviceBuffersUpToDate;
  mutable host_buffer_type                            _hostBuffer;
  // _deviceBuffers empty => buffers not created yet
  mutable std::map< detail::Device::id_type,
                    detail::DeviceBuffer >            _deviceBuffers;
};

template <typename T>
RegisterVectorDeviceFunctions<T> Vector<T>::registerVectorDeviceFunctions;

} // namespace skelcl

#include "detail/VectorDef.h"

#endif // VECTOR_H_
