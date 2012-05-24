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
/// \file ZipDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef ZIP_DEF_H_
#define ZIP_DEF_H_

#include <algorithm>
#include <istream>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#undef  __CL_ENABLE_EXCEPTIONS

#include "../Distribution.h"
#include "../Out.h"
#include "../Source.h"

#include "Assert.h"
#include "Container.h"
#include "Device.h"
#include "KernelUtil.h"
#include "Logger.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

namespace skelcl {

template<typename Tleft, typename Tright, typename Tout>
Zip<Tout(Tleft, Tright)>::Zip(const Source& source,
                              const std::string& funcName)
  : detail::Skeleton(), _program(this->createProgram(source))
{
  prepareAndBuildProgram(funcName);
  LOG_DEBUG("Create new Zip object (", this, ")");
}

template<typename Tleft, typename Tright, typename Tout>
detail::Program
  Zip<Tout(Tleft, Tright)>::createProgram(const std::string& source) const
{
  ASSERT_MESSAGE(!source.empty(),
    "Tried to create program with empty user source.");
  // first: user defined source
  std::string s(source);
  // second: append skeleton implementation source
  s.append(R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;
typedef float SCL_TYPE_2;

__kernel void SCL_ZIP(
    const __global SCL_TYPE_0*  SCL_LEFT,
    const __global SCL_TYPE_1*  SCL_RIGHT,
          __global SCL_TYPE_2*  SCL_OUT,
    const unsigned int          SCL_ELEMENTS ) {
  if (get_global_id(0) < SCL_ELEMENTS) {
    SCL_OUT[get_global_id(0)] = SCL_FUNC(SCL_LEFT[get_global_id(0)],
                                         SCL_RIGHT[get_global_id(0)]);
  }
}
)");
  return detail::Program(s, detail::util::hash("//Zip\n"+source));
}

template<typename Tleft, typename Tright, typename Tout>
void
  Zip<Tout(Tleft, Tright)>::prepareAndBuildProgram(const std::string& funcName)
{
  if (!_program.loadBinary()) {
    _program.transferParameters(funcName, 2, "SCL_ZIP");
    _program.transferArguments(funcName, 2, "SCL_FUNC");
    _program.renameFunction(funcName, "SCL_FUNC");
    _program.adjustTypes<Tleft, Tright, Tout>();
  }
  _program.build();
}

template <typename Tleft, typename Tright, typename Tout>
template <template <typename> class ContainerType,
          typename... Args>
ContainerType<Tout>
  Zip<Tout(Tleft, Tright)>::operator()(const ContainerType<Tleft>& left,
                                       const ContainerType<Tright>& right,
                                       Args&&... args)
{
  ContainerType<Tout> output;
  this->operator()(out(output), left, right, std::forward<Args>(args)...);
  return output;
}

template <typename Tleft, typename Tright, typename Tout>
template <template <typename> class ContainerType,
          typename... Args>
ContainerType<Tout>&
  Zip<Tout(Tleft, Tright)>::operator()(Out<ContainerType<Tout>> output,
                                       const ContainerType<Tleft>& left,
                                       const ContainerType<Tright>& right,
                                       Args&&... args)
{
  static_assert(std::is_base_of<skelcl::detail::Container<Tleft>,
                ContainerType<Tleft>>::value,
      "First argument is no derived class of skelcl::detail::Container");
  static_assert(std::is_base_of<skelcl::detail::Container<Tright>,
                ContainerType<Tright>>::value,
      "Second argument is no derived class of skelcl::detail::Container");
  ASSERT(left.size() <= right.size());

  prepareInput(left, right);

  prepareOutput(output.container(), left, right);

  execute(output.container(), left, right, std::forward<Args>(args)...);

  updateModifiedStatus(output, std::forward<Args>(args)...);

  return output.container();
}

template <typename Tleft, typename Tright, typename Tout>
void Zip<Tout(Tleft, Tright)>::prepareInput(const detail::Container<Tleft>& left,
                                            const detail::Container<Tright>& right)
{
  // set default distribution if required
  if (   left.distribution()  == nullptr
      && right.distribution() == nullptr ) {
    left.setDistribution(Distribution::Block());
    right.setDistribution(Distribution::Block());
  } else if (left.distribution() == nullptr) {
    left.setDistribution(right.distribution());
  } else if (right.distribution() == nullptr) {
    right.setDistribution(left.distribution());
  } else if ( left.distribution() != right.distribution() ) {
    // TODO: find a better solution
    left.setDistribution(Distribution::Block());
    right.setDistribution(Distribution::Block());
  }
  // create buffers if required
  left.createDeviceBuffers();
  right.createDeviceBuffers();
  // copy data to devices
  left.startUpload();
  right.startUpload();
}

template <typename Tleft, typename Tright, typename Tout>
void
  Zip<Tout(Tleft, Tright)>::prepareOutput(detail::Container<Tout>& output,
                                          const detail::Container<Tleft>& left,
                                          const detail::Container<Tright>& right)
{
  if (   static_cast<void*>(&output) == static_cast<const void*>(&left)
      || static_cast<void*>(&output) == static_cast<const void*>(&right) ) {
    return; // already prepared in prepareInput
  }
  // resize container if required
  if (output.size() < left.size()) {
    output.resize(left.size());
  }
  // adopt distribution from left input
  output.setDistribution(left.distribution());
  // create buffers if required
  output.createDeviceBuffers();
}

template <typename Tleft, typename Tright, typename Tout>
template <typename... Args>
void Zip<Tout(Tleft, Tright)>::execute(detail::Container<Tout>& output,
                                       const detail::Container<Tleft>& left,
                                       const detail::Container<Tright>& right,
                                       Args&&... args)
{
  ASSERT( left.distribution()->isValid() && right.distribution()->isValid() );
  ASSERT( *(left.distribution()) == *(right.distribution()) );
  ASSERT( left.size() >= right.size() && output.size() >= left.size() );

  for (auto& devicePtr : left.distribution()->devices()) {
    auto& outputBuffer= output.deviceBuffer(*devicePtr);
    auto& leftBuffer  = left.deviceBuffer(*devicePtr);
    auto& rightBuffer = right.deviceBuffer(*devicePtr);

    cl_uint elements  = leftBuffer.size();
    cl_uint local     = std::min(this->workGroupSize(),
                                 devicePtr->maxWorkGroupSize());
    cl_uint global    = detail::util::ceilToMultipleOf(elements, local);

    try {
      cl::Kernel kernel(this->_program.kernel(*devicePtr, "SCL_ZIP"));

      kernel.setArg(0, leftBuffer.clBuffer());
      kernel.setArg(1, rightBuffer.clBuffer());
      kernel.setArg(2, outputBuffer.clBuffer());
      kernel.setArg(3, elements);

      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 4,
                                        std::forward<Args>(args)...);

      // after finishing the kernel invoke this function ...
      auto invokeAfter =  [&] () {
                            leftBuffer.markAsNotInUse();
                            rightBuffer.markAsNotInUse();
                            outputBuffer.markAsNotInUse();
                          };

      devicePtr->enqueue(kernel,
                         cl::NDRange(global), cl::NDRange(local),
                         cl::NullRange, // offset
                         invokeAfter);

      // after successfully enqueued the kernel ...
      leftBuffer.markAsInUse();
      rightBuffer.markAsInUse();
      outputBuffer.markAsInUse();
      
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
  }
  LOG_INFO("Zip kernel started");
}

} // namespace skelcl

#endif // ZIP_DEF_H_

