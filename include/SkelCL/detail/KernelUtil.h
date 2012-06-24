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
/// \file KernelUtil.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef KERNEL_UTIL_H_
#define KERNEL_UTIL_H_

#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#undef  __CL_ENABLE_EXCEPTIONS

#include "../Out.h"
#include "../Vector.h"

#include "Device.h"
#include "DeviceBuffer.h"
#include "Logger.h"

namespace skelcl {

namespace detail {

namespace kernelUtil {

template <typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   Args&&... args);

// Declarations

template <typename T, typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   Out<Vector<T>>&& vector,
                   Args&&... args);

template <typename T, typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   Vector<T>& vector,
                   Args&&... args);

template <typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   skelcl::detail::Sizes& sizes,
                   Args&&... args);

template <typename T, typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   T value,
                   Args&&... args);

// Definitions

void setKernelArgs(cl::Kernel& /*kernel*/,
                   const Device& /*device*/,
                   size_t /*index*/)
{
}

template <typename T, typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   Out<Vector<T>>&& outVector,
                   Args&&... args)
{
  setKernelArgs( kernel, device, index,
                 outVector.container(), std::forward<Args>(args)... );
}

template <typename T, typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   Vector<T>& vector,
                   Args&&... args)
{
  try {
    kernel.setArg( index, vector.deviceBuffer(device).clBuffer() );
  } catch (cl::Error& err) {
    LOG_ERROR("Error while setting argument ", index,
      " (Vector version called)");
    ABORT_WITH_ERROR(err);
  }
  setKernelArgs( kernel, device, ++index, std::forward<Args>(args)... );
}

template <typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   skelcl::detail::Sizes& sizes,
                   Args&&... args)
{
  try {
    kernel.setArg( index, sizes[device.id()]);
  } catch (cl::Error& err) {
    LOG_ERROR("Error while setting argument ", index,
      " (Sizes version called)");
    ABORT_WITH_ERROR(err);
  }
  setKernelArgs( kernel, device, ++index, std::forward<Args>(args)... );
}

template <typename T, typename... Args>
void setKernelArgs(cl::Kernel& kernel,
                   const Device& device,
                   size_t index,
                   T value,
                   Args&&... args)
{
  try {
    kernel.setArg( index, value );
  } catch (cl::Error& err) {
    LOG_ERROR("Error while setting argument ", index,
      " (Default version called)");
    ABORT_WITH_ERROR(err);
  }
  setKernelArgs( kernel, device, ++index, std::forward<Args>(args)... );
}

} // namespace kernelUtil

} // namespace detail

} // namespace skelcl

#endif // KERNEL_UTIL_H_

