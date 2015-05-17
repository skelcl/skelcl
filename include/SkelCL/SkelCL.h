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
/// \file SkelCL.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///
/// \mainpage
///
/// \section intro Introduction
///
/// This document describes SkelCL a multi-GPU skeleton library based
/// on OpenCL. It is actively developed at the University of Münster, Germany,
/// by Michel Steuwer and others. For comments or questions feel
/// free to contact the author: <michel.steuwer@uni-muenster.de>
///
/// Most of the functionality is explained in multiple peer-reviewed
/// publications and in detail in a diploma thesis with the title:
/// "Developing a Portable Multi-GPU Skeleton Library".
///
/// Two key abstractions are made in this library: algorithmic skeletons
/// and a unified memory abstraction. This also reflects in the classes
/// described here. The Skeleton class and subclasses describe
/// calculations performed on a GPU.
/// The Container classes, like Vector provides a unified memory management
/// for CPU and GPU memory.
///
/// \section example Example
///
/// The following example shows a full functional SkelCL program.
/// The dot product of two vectors is calculated.
/// The Zip skeleton combines two vectors element-wise.
/// The Reduce skeleton reduces all elements of a vector to a single
/// value.
///
/// \code
///
/// #include <SkelCL/SkelCL.h>
/// #include <SkelCL/Zip.h>
/// #include <SkelCL/Reduce.h>
/// #include <SkelCL/Vector.h>
///
/// #include <iostream>
///
/// using namespace skelcl;
///
/// int main () {
///   skelcl::init(); // Initialize SkelCL
///
///   // Create zip skeleton with the multiplication as operation
///   Zip<float(float)> mult("(float x, float y){ return x*y; }");
///
///   // Reduce skeleton requires the identity of the given operation as
///   // second argument. For the addition this is 0.
///   Reduce<float(float)> sum("(float x, float y){ return x+y; }", "0");
///
///   // Create vectors A and B
///   Vector<float> A(1024); init(A.begin(), A.end());
///   Vector<float> B(1024); init(B.begin(), B.end());
///
///   // Execute the skeletons
///   Vector<float> C = sum( mult(A, B) );
///
///   // Access the calculated value.
///   std::cout << "Dot product: " C.front() << std::endl;
/// }
///
/// \endcode
///
///

///
/// \defgroup skeletons Algorithmic Skeletons
/// \brief An overview of all algorithmic skeletons offered by SkelCL.
///
/// \defgroup containers Containers
/// \brief An overview of all container data types offered by SkelCL.
///
/// \defgroup distributions Distributions
/// \brief An overview of all distributions offered by SkelCL. 
///

///
/// \namespace skelcl
/// \brief  The namespace skelcl encapsulated the whole project.
///         Every class provided by SkelCL is in this namespace.
///

#ifndef SKELCL_H_
#define SKELCL_H_

#include "detail/Device.h"
#include "detail/DeviceID.h"
#include "detail/DeviceProperties.h"
#include "detail/Macros.h"
#include "detail/PlatformID.h"
#include "detail/Types.h"
#include "detail/skelclDll.h"

namespace skelcl {

///
/// \brief Public name for a type representing different types of device.
///        E.g. CPU or GPU.
///
typedef detail::Device::Type device_type;

///
/// \brief Creates a detail::DeviceProperties object representing all devices in
///        the system. This object should be used as parameter of the
///        init(detail::DeviceProperties) function.
///
SKELCL_DLL detail::DeviceProperties allDevices();

///
/// \brief Creates a detail::DeviceProperties object representing n devices.
///        This object should be used as parameter of the
///        init(detail::DeviceProperties) function.
///
/// \param n Number of OpenCL devices.
///
SKELCL_DLL detail::DeviceProperties nDevices(size_t n);

///
/// \brief Creates an OpenCL platform ID to be used as parameter of the
///        init(detail::PlatformID, detail::DeviceID) function.
///
/// \param pID The ID of the OpenCL platform.
///
SKELCL_DLL detail::PlatformID platform(size_t pID);

///
/// \brief Creates an OpenCL device ID to be used as parameter of the
///        init(detail::PlatformID, detail::DeviceID) function.
///
/// \param dID The ID of the OpenCL device.
///
SKELCL_DLL detail::DeviceID device(size_t dID);

///
/// \brief Initializes the SkelCL library. This function (or another init
///        function) has to be called prior to every other function in the
///        library.
///
/// \param properties Specifies properties about the devices to use by default
///        all devices are selected by calling the allDevices() function. To
///        select a specific number of devices call nDevices(size_t). Additional
///        properties can be specified using member functions of the
///        detail::DeviceProperties class.
///
/// \sa DeviceSelectionTests.cpp
///
SKELCL_DLL void init(detail::DeviceProperties properties = allDevices());

///
/// \brief Initializes the SkelCL library. This function (or another init
///        function) has to be called prior to every other function in the
///        library.
///
/// This function should only be used if one specific OpenCL device should be
/// used by SkelCL.
///
/// \param pID The ID of the OpenCL platform to be used.
/// \param dID The ID of the OpenCL device to be used.
///
SKELCL_DLL void init(detail::PlatformID pID, detail::DeviceID dID);

///
/// \brief Frees all resources allocated internally by SkelCL.
///
/// This function normally does not have to be called explicitly.
/// If for some reason the number of devices used by SkelCL should be changed
/// after a first initialization, this function has to be called prior to
/// calling one of the init() functions again.
///
SKELCL_DLL void terminate();

} // namespace skelcl

#endif // SKELCL_H_
