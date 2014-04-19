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
/// \file DeviceList.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef DEVICE_LIST_H_
#define DEVICE_LIST_H_

#include <initializer_list>
#include <memory>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include "Device.h"
#include "skelclDll.h"

namespace skelcl {

namespace detail {

class DeviceID;
class DeviceProperties;
class PlatformID;

class SKELCL_DLL DeviceList {
  typedef std::shared_ptr<Device> device_ptr;
  typedef std::vector<device_ptr> vector_type;
public:
  typedef vector_type::value_type value_type;
  typedef vector_type::reference reference;
  typedef vector_type::const_reference const_reference;
  typedef vector_type::iterator iterator;
  typedef vector_type::const_iterator const_iterator;
  typedef vector_type::reverse_iterator reverse_iterator;
  typedef vector_type::const_reverse_iterator const_reverse_iterator;
  typedef vector_type::size_type size_type;

  DeviceList();
  // DeviceList(); = default;
  // throws _devices should be initialized in the member
  // initialization list warning

  DeviceList(std::initializer_list<std::shared_ptr<Device>> list);

  //DeviceList(const DeviceList&) = default;

  //DeviceList& operator=(const DeviceList&) = default;

  //~DeviceList() = default;

  bool operator==(const DeviceList& rhs) const;

  void init(DeviceProperties properties);

  void init(PlatformID pID, DeviceID dID);

  void clear();

  void barrier() const;

  const_iterator begin() const;

  const_iterator end() const;

  const_reverse_iterator rbegin() const;

  const_reverse_iterator rend() const;

  size_type size() const;

  bool empty() const;

  const_reference operator[](size_type n) const;

  const_reference at(size_type n) const;

  const_reference front() const;

  const_reference back() const;

private:
  vector_type _devices;
};

SKELCL_DLL extern DeviceList globalDeviceList;

} // namespace detail

} // namespace skelcl

#endif // DEVICE_LIST_H_

