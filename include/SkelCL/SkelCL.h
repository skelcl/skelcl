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
 // #include <iostream>
/// #include <SkelCL/SkelCL.h>
///
/// using namespace skelcl;
///
/// int main (int argc, char const *argv[]) {
///   skelcl::init(); // Initialize SkelCL
///   // Create zip skeleton with the multiplication as operation
///   Zip<float(float)> mult("(float x, float y){ return x*y; }");
///   // Reduce skeleton requires the identity of the given operation as
///   // second argument. For the addition this is 0.
///   Reduce<float(float)> sum("(float x, float y){ return x+y; }", "0");
///
///   int size = 1024;
///   // Fill pointers a_ptr and b_ptr with random values.
///   float* a_ptr = mallocAndInit(size);
///   float* b_ptr = mallocAndInit(size);
///
///   // Create vectors A and B
///   Vector<float> A(a_ptr, a_ptr+size);
///   Vector<float> B(b_ptr, b_ptr+size);
///
///   // Execute the skeletons
///   Vector<float> C = sum( mult(A, B) );
///
///   // Access the calculated value.
///   std::cout << C.front() << std::endl;
/// }
///
/// \endcode
///
///

///
/// \namespace skelcl
/// \brief  The namespace skelcl encapsulated the whole project.
///         Every class provided by SkelCL is in this namespace.
///

#ifndef SKELCL_H_
#define SKELCL_H_

#include "SkelCL/detail/Device.h"
#include "SkelCL/detail/DeviceProperties.h"

namespace skelcl {

typedef detail::Device::Type device_type;

detail::DeviceProperties allDevices();

detail::DeviceProperties nDevices(size_t n);

///
/// \brief Initializes the SkelCL library. This function has to be called
///        prior to every other function in the library.
///
/// \param properties Specifies properties about the devices to use
///                   by default all devices are selected by calling the
///                   allDevices() function. To select a specific number
///                   of devices call nDevices(n)
///
void init(detail::DeviceProperties properties = allDevices());

///
/// \brief Frees all resources allocated internally by SkelCL.
///
/// This function normally does not have to be called explicitly.
/// If for some reason the number of devices used by SkelCL should be changed
/// after a first initialization, this function has to be called prior to
/// calling the init function again.
///
void terminate();

} // namespace skelcl

#endif // SKELCL_H_
