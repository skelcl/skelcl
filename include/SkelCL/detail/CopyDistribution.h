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
/// \file CopyDistribution.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef COPY_DISTRIBUTION_H_
#define COPY_DISTRIBUTION_H_

#include "DeviceList.h"

#include "../Distribution.h"

namespace skelcl {

namespace detail {

template <typename T>
class CopyDistribution : public skelcl::Distribution {
public:
  CopyDistribution() = delete;
  CopyDistribution(const DeviceList& deviceList,
                   std::function<T(const T&, const T&)> combineFunc = nullptr);
  CopyDistribution(const CopyDistribution&) = default;
  ~CopyDistribution();
  CopyDistribution& operator=(const CopyDistribution&) = default;

  bool isCopy() const;

  void startUpload(const std::vector<detail::DeviceBuffer>& deviceBuffers,
                   void* const hostPointer,
                   detail::Event* events) const;

  void startDownload(const std::vector<detail::DeviceBuffer>& deviceBuffers,
                     void* const hostPointer,
                     detail::Event* events) const;

  size_t sizeForDevice(const detail::Device::id_type deviceID,
                       const size_t totalSize) const;

  std::function<T(const T&, const T&)> combineFunc() const;

protected:
  bool doCompare(const Distribution& rhs) const;

private:
  std::function<T(const T&, const T&)> _combineFunc;
};


template <>
class CopyDistribution<void> : public skelcl::Distribution {
public:
  CopyDistribution() = delete;
  CopyDistribution(const DeviceList& deviceList);
  CopyDistribution(const CopyDistribution&) = default;
  ~CopyDistribution();
  CopyDistribution& operator=(const CopyDistribution&) = default;

  bool isCopy() const;

  void startUpload(const std::vector<detail::DeviceBuffer>& deviceBuffers,
                   void* const hostPointer,
                   detail::Event* events) const;

  void startDownload(const std::vector<detail::DeviceBuffer>& deviceBuffers,
                     void* const hostPointer,
                     detail::Event* events) const;

  size_t sizeForDevice(const detail::Device::id_type deviceID,
                       const size_t totalSize) const;

protected:
  bool doCompare(const Distribution& rhs) const;
};

} // namespace detail

} // namespace skelcl

#include "CopyDistributionDef.h"

#endif // COPY_DISTRIBUTION_H_
