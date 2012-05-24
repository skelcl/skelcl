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
/// \file BlockDistribution.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef BLOCK_DISTRIBUTION_H_
#define BLOCK_DISTRIBUTION_H_

#include "../Distribution.h"

#include "Significances.h"

namespace skelcl {

namespace detail {

class DeviceList;

class BlockDistribution : public skelcl::Distribution {
public:
  BlockDistribution() = default;
  BlockDistribution( const DeviceList& deviceList );
  BlockDistribution( const DeviceList& deviceList,
                     const Significances& significances );
  BlockDistribution(const BlockDistribution&) = default;
  ~BlockDistribution() = default;
  BlockDistribution& operator=(const BlockDistribution&) = default;

  bool isBlock() const;

  void startUpload(const std::vector<detail::DeviceBuffer>& deviceBuffers,
                   void* const hostPointer,
                   detail::Event* events) const;

  void startDownload(const std::vector<detail::DeviceBuffer>& deviceBuffers,
                     void* const hostPointer,
                     detail::Event* events) const;

  size_t sizeForDevice(const detail::Device::id_type deviceID,
                       const size_t totalSize) const;

  const Significances& getSignificances() const;

private:
  bool doCompare(const Distribution& rhs) const;

  Significances _significances;
};

} // namespace detail

} // namespace skelcl

#endif // BLOCK_DISTRIBUTION_H_

