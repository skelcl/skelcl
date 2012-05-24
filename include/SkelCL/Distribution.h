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

#include "detail/Device.h"
#include "detail/DeviceBuffer.h"
#include "detail/DeviceList.h"
#include "detail/Event.h"
#include "detail/Significances.h"

namespace skelcl {

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
class Distribution {
public:
  ///
  /// \brief No default constructor
  ///
  Distribution() = delete;

  ///
  /// \brief Default copy constructor
  ///
  Distribution(const Distribution& rhs);

  ///
  /// \brief Default destructor
  ///
  virtual ~Distribution();

  ///
  /// \brief Default assignment operator
  ///
  Distribution& operator=(const Distribution& rhs);

  ///
  /// \brief Create a new single distribution.
  ///
  /// The data is copied to a single device only.
  ///
  /// \param deviceID A valid device id identifying a device
  ///                 to which the data is copied
  ///
  /// \return A pointer pointing to a valid single distribution
  ///
  static std::shared_ptr<Distribution> Single(const detail::Device::id_type deviceID);

  ///
  /// \brief Create a new block distribution.
  ///
  /// The data is split evenly across all devices and parts are copied to all
  /// devices available.
  ///
  /// \return A pointer pointing to a valid block distribution
  ///
  static std::shared_ptr<Distribution> Block();

  ///
  /// \brief Create a new block distribution.
  ///
  /// The data is split across the given devices as specified by the given
  /// significances.
  ///
  /// \param deviceList     Specifies the devices involved in the distribution
  ///        significances  Specifies how many data each devices gets
  ///
  /// \return A pointer pointing to a valid block distribution
  ///
  static std::shared_ptr<Distribution>
    Block(const detail::DeviceList& deviceList,
          const detail::Significances& significances);

  ///
  /// \brief Create a new copy distribution.
  ///
  /// The data is copied to all devices available.
  /// When changing from this distribution, the data residing on the first
  /// device is chosen as the data distributed by the new selected
  /// distribution.
  ///
  /// \return A pointer pointing to a valid copy distribution
  ///
  static std::shared_ptr<Distribution> Copy();

  ///
  /// \brief Create a new copy distribution
  ///
  /// The data is copied to all devices available.
  /// When changing from this distribution, the data from all devices is
  /// combined by applying the provided function pairwise on the data
  /// items.
  ///
  /// \param func This function is applied pairwise on the data items to combine
  ///             the data from different devices into one
  ///
  /// \return A pointer pointing to a valid copy distribution
  ///
  template <typename T>
  static std::shared_ptr<Distribution> Copy(T(*func)(const T&, const T&));

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
  /// \brief States if the distribution is single
  ///
  /// \return Returns true only if the distribution is single.
  ///
  virtual bool isSingle() const;

  ///
  /// \brief States if the distribution is block
  ///
  /// \return Returns true only if the distribution is block.
  ///
  virtual bool isBlock() const;

  ///
  /// \brief States if the distribution is copy
  ///
  /// \return Returns true only if the distribution is copy.
  ///
  virtual bool isCopy() const;

  ///
  /// \brief States if the distribution is valid, i.e., single, block or copy
  ///
  /// \return Returns true if the distribution is either single, block or copy
  ///
  bool isValid() const;

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
  virtual void startUpload(const std::vector<detail::DeviceBuffer>& deviceBuffers,
                           void* const hostPointer,
                           detail::Event* events) const = 0;

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
  virtual void startDownload(const std::vector<detail::DeviceBuffer>& deviceBuffers,
                             void* const hostPointer,
                             detail::Event* events) const = 0;

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
  virtual size_t sizeForDevice(const detail::Device::id_type deviceID,
                               const size_t totalSize) const = 0;

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

} // namespace skelcl

#include "detail/CopyDistribution.h"
// include implementation of the templated copy distribution, before using it

namespace skelcl {

template <typename T>
std::shared_ptr<Distribution> Distribution::Copy(T(*func)(const T&, const T&))
{
  return std::make_shared<
            detail::CopyDistribution<T> >( detail::globalDeviceList, func );
}

} // namespace skelcl


#endif // DISTRIBUTION_H_
