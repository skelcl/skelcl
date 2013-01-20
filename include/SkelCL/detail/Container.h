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
/// \file Container.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <memory>

namespace skelcl {

template <typename> class Distribution;

namespace detail {

class Device;
class DeviceBuffer;
class Event;

///
/// \class Container
///
/// \brief Abstract base class for skelcl containers
///
/// \tparam T Type of the value stored in the container
///
/// Common function for containers like Vector<T> or Matrix<T>
///
template <typename T>
class Container {
public:
  ///
  /// \brief Default constructor
  ///
  ///
  Container() = default;

  ///
  /// \brief Default copy constructor
  ///
  Container(const Container<T>&) = default;

  ///
  /// \brief Default virtual destructor
  ///
  virtual ~Container() {};

  ///
  /// \brief Default assignment operator
  ///
  /// \return Reference to the assigned container
  ///
  Container& operator=(const Container<T>&) = default;

  ///
  /// \brief Access Distribution of the container
  ///
  /// \return Distribution of the container
  ///
  virtual Distribution& distribution() const = 0;

  ///
  /// \brief Set the Distribution of the container
  ///
  /// \param distribution Distribution to be set
  ///
  virtual void setDistribution(const Distribution& distribution) const = 0;

  ///
  /// \brief Create buffers on the devices depending on the distribution
  ///
  /// \sa DeviceBuffer
  ///
  virtual void createDeviceBuffers() const = 0;

  ///
  /// \brief Force the creation of the buffers on the devices
  ///        depending on the distribution
  ///
  /// The buffers are created even if buffers are already present
  /// on the devices. In this case the present buffers are deleted
  /// and new ones are created.
  ///
  /// \sa DeviceBuffer
  ///
  virtual void forceCreateDeviceBuffers() const = 0;

  ///
  /// \brief Start copying data from the device buffers to the host
  ///
  /// \return An Event is returned which can be used
  ///         to wait until the copying is finished.
  ///
  virtual detail::Event startDownload() const = 0;

  ///
  /// \brief Start copying data from the host to the device buffers
  ///
  /// \return An Event is returned which can be used
  ///         to wait until the copying is finished.
  ///
  virtual detail::Event startUpload() const = 0;

  ///
  /// \brief Notifies the container that the data on the device is
  ///        now up to date.
  ///
  /// This implies that the data on the host is out of date.
  ///
  virtual void dataOnDeviceModified() const = 0;

  ///
  /// \brief Notifies the container that the data on the host is
  ///        now up to date.
  ///
  /// This implies that the data on the devices is out of date.
  ///
  virtual void dataOnHostModified() const = 0;

  ///
  /// \brief Returns the number of elements inside the container.
  ///
  /// \return The number of elements stored in the container.
  ///
  virtual size_t size() const = 0;

  ///
  /// \brief Resizes the container to hold sz elements.
  ///        If sz is smaller than the current size, the first sz
  ///        elements remain untouched, the rest is dropped.
  ///        If sz is greater than the current size, the container is
  ///        expanded with copies of c.
  ///        If sz is equal to the current size, the call has no effects.
  ///
  virtual void resize(size_t sz, T c = T()) = 0;

  ///
  /// \brief Returns the DeviceBuffer for the given Device
  ///
  /// \param device Device for which the DeviceBuffer is returned
  /// \return DeviceBuffer for the given Device. If the container
  ///         doesn't hold an buffer for the device an invalid
  ///         DeviceBuffer is returned.
  ///
  virtual const detail::DeviceBuffer& deviceBuffer(const detail::Device&
                                                      device) const = 0;
};

} // namespace detail

} // namespace skelcl

#endif // CONTAINER_H_

