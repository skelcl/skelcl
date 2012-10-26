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
/// \file Distribution.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef DISTRIBUTION_H_
#define DISTRIBUTION_H_

#include <functional>
#include <memory>
#include <string>

#include "Device.h"
#include "DeviceBuffer.h"
#include "DeviceList.h"
#include "Event.h"

namespace skelcl {

namespace detail {

///
/// \class Distribution
///
/// \brief A distribution describes on which devices and how the
///        data of a container is copied.
///
/// Currently three different distributions are possible.
/// They are accessible through the static member functions
/// Single, Block and Copy
///
template <typename> class Distribution;

template <template <typename> class C, typename T>
class Distribution< C<T> > {
public:
  ///
  /// \brief Default constructor
  ///
  Distribution();

  ///
  /// \brief Copy constructor from a distribution of the same container,
  ///        but arbitrary types stored in the container
  ///
  template <typename U>
  Distribution(const Distribution< C<U> >& rhs);

  template <typename U>
  Distribution(Distribution< C<U> >&& rhs);

  ///
  /// \brief Default destructor
  ///
  virtual ~Distribution();

  ///
  /// \brief Assignment operator from a distribution of the same container,
  ///        but arbitrary types stored in the container
  ///
  template <typename U>
  Distribution& operator=(const Distribution< C<U> >& rhs);

  template <typename U>
  Distribution& operator=(Distribution< C<U> >&& rhs);

  static std::shared_ptr<Distribution> Overlap();

  ///
  /// \brief Comparison operator
  ///
  /// \param rhs Distribution to be compared with.
  ///
  /// \return Returns true only if the distribution rhs is equal to
  ///         this.
  ///         Two distributions are considered equal if:
  ///         both distributions are block distributions
  ///         or:
  ///         both distributions are single distributions and
  ///         they both use the same device
  ///         or:
  ///         both distributions are copy distributions
  ///
  bool operator==(const Distribution& rhs) const;

  ///
  /// \brief Unequal comparison operator
  ///
  /// \param rhs Distribution to be compared with.
  ///
  /// \return Returns !operator==(rhs)
  ///
  bool operator!=(const Distribution& rhs) const;

  ///
  /// \brief States if the distribution is valid, i.e., single, block or copy
  ///
  /// \return Returns true if the distribution is either single, block or copy
  ///
  virtual bool isValid() const;

  ///
  /// \brief Starts copying data pointed by hostPointer to the deviceBuffers
  ///        according to the specific distribution.
  ///
  /// \param deviceBuffers  Buffers for all devices involved in the
  ///                       distribution
  ///        hostPotiner    Pointer pointing to the data to be copied
  ///                       from
  ///        events         Event object to allow for explicitly waiting
  ///                       for the copy operation to be completed
  ///
  virtual void startUpload(C<T>& container,
                           detail::Event* events) const;

  ///
  /// \brief Starts copying data from the deviceBuffers and storing the
  ///        downloaded data in the devicePointer according to the
  ///        specific distribution.
  ///
  /// \param deviceBuffers  Buffers for all devices involved in the
  ///                       distribution
  ///        hostPotiner    Pointer pointing to the memory where the
  ///                       data should be stored
  ///        events         Event object to allow for explicitly waiting
  ///                       for the copy operation to be completed
  ///

  virtual void startDownload(C<T>& container,
                             detail::Event* events) const;

  ///
  /// \brief Returns a list of all devices to which data should be
  ///        distributed in the current distribution.
  ///
  /// \return A list of all devices included in the distribution.
  ///         For a single distribution devices returns a list
  ///         with only a single device. For a block or copy
  ///         distribution a list with all available devices is returned.
  ///
  const detail::DeviceList& devices() const;

  ///
  /// \brief Returns a shared pointer to the device at position device
  ///
  /// \param device The device id to which the reference should be
  ///               returned. If the distribution is single only for 0 a
  ///               valid pointer is returned.
  ///               If the distribution is block or copy for a value
  ///               between 0 and the total number of devices a valid
  ///               pointer is returned.
  ///
  /// \return Shared pointer to the device at position device
  ///
  const detail::DeviceList::value_type device(const size_t device) const;

  ///
  /// \brief Returns the number of elements to be stored on the given device
  ///        according to the distribution
  ///
  /// \param deviceID  The device for which the number of elements to be stored
  ///                  on it should be returned
  ///        totalSize The total size of the vector, so that the distribution
  ///                  can decide how large the part for the given device should
  ///                  be
  ///
  /// \return The number of elements to be stored on the given device
  ///
  virtual size_t sizeForDevice(const C<T>& container,
                               const detail::Device::id_type id) const;

  virtual bool dataExchangeOnDistributionChange(Distribution& newDistribution);

protected:
  ///
  /// \brief Constructor used by derived classes
  ///
  /// \param deviceList List of devices to be involved in the distribution
  ///
  Distribution(const detail::DeviceList& deviceList);

  ///
  /// \brief Formates information about the current instance into a string,
  ///        used for Debug purposes
  ///
  /// \return A formated string with information about the current instance
  ///
  virtual std::string getInfo() const;

  ///
  /// List of devices involved in the distribution
  detail::DeviceList  _devices;

private:
  ///
  /// \brief This function is invoked to do a comparison, since operator== can
  ///        not be virtual
  ///
  /// \param rhs The distribution the current instance is compared to
  ///
  /// \return Returns true if this and rhs are equal, false otherwise.
  ///         (See operator== for more details)
  ///
  virtual bool doCompare(const Distribution& rhs) const;
};

} // namespace detail

} // namespace skelcl

#include "DistributionDef.h"

#endif // DISTRIBUTION_H_
