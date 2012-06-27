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
/// \file MapDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef MAP_DEF_H_
#define MAP_DEF_H_

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

#include "../Distributions.h"
#include "../Out.h"
#include "../Source.h"

#include "Assert.h"
#include "Device.h"
#include "KernelUtil.h"
#include "Logger.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

namespace skelcl {

template<typename Tin, typename Tout>
Map<Tout(Tin)>::Map(const Source& source,
                    const std::string& funcName)
  : Skeleton(),
    _source(source),
    _funcName(funcName)
{
  LOG_DEBUG("Create new Map object (", this, ")");
}

template <typename Tin, typename Tout>
template <template <typename> class C,
          typename... Args>
C<Tout> Map<Tout(Tin)>::operator()(const C<Tin>& input,
                                   Args&&... args)
{
  C<Tout> output;
  this->operator()(out(output), input, std::forward<Args>(args)...);
  return output;
}

template <typename Tin, typename Tout>
template <template <typename> class C,
          typename... Args>
C<Tout>& Map<Tout(Tin)>::operator()(Out< C<Tout> > output,
                                    const C<Tin>& input,
                                    Args&&... args)
{
  auto program = createAndBuildProgram<C>();

  this->prepareInput(input);

  prepareAdditionalInput(std::forward<Args>(args)...);

  this->prepareOutput(output.container(), input);

  execute(program, output.container(), input, std::forward<Args>(args)...);

  updateModifiedStatus(output, std::forward<Args>(args)...);

  return output.container();
}

template <typename Tin, typename Tout>
template <template <typename> class C,
          typename... Args>
void Map<Tout(Tin)>::execute(const detail::Program& program,
                             C<Tout>& output,
                             const C<Tin>& input,
                             Args&&... args)
{
  ASSERT( input.distribution().isValid() );
  ASSERT( output.size() >= input.size() );

  for (auto& devicePtr : input.distribution().devices()) {
    auto& outputBuffer= output.deviceBuffer(*devicePtr);
    auto& inputBuffer = input.deviceBuffer(*devicePtr);

    cl_uint elements  = inputBuffer.size();
    cl_uint local     = std::min(this->workGroupSize(),
                                 devicePtr->maxWorkGroupSize());
    cl_uint global    = detail::util::ceilToMultipleOf(elements, local);

    try {
      cl::Kernel kernel(program.kernel(*devicePtr, "SCL_MAP"));

      kernel.setArg(0, inputBuffer.clBuffer());
      kernel.setArg(1, outputBuffer.clBuffer());
      kernel.setArg(2, elements);

      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 3,
                                        std::forward<Args>(args)...);

      // after finishing the kernel invoke this function ...
      auto invokeAfter =  [&] () {
                            inputBuffer.markAsNotInUse();
                            outputBuffer.markAsNotInUse();
                          };

      devicePtr->enqueue(kernel,
                         cl::NDRange(global), cl::NDRange(local),
                         cl::NullRange, // offset
                         invokeAfter);

      // after successfully enqueued the kernel ...
      inputBuffer.markAsInUse();
      outputBuffer.markAsInUse();
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
  }
  LOG_INFO("Map kernel started");
}

template <typename Tin, typename Tout>
template <template <typename> class C>
detail::Program Map<Tout(Tin)>::createAndBuildProgram() const
{
  ASSERT_MESSAGE(!_source.empty(),
    "Tried to create program with empty user source.");

  // create program

  std::string s(C<Tin>::deviceFunctions());
  // first: user defined source
  s.append(_source);
  // second: append skeleton implementation source
  s.append(R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;

__kernel void SCL_MAP(
    const __global SCL_TYPE_0*  SCL_IN,
          __global SCL_TYPE_1*  SCL_OUT,
    const unsigned int          SCL_ELEMENTS)
{
  if (get_global_id(0) < SCL_ELEMENTS) {
    SCL_OUT[get_global_id(0)] = SCL_FUNC(SCL_IN[get_global_id(0)]);
  }
}
)");
  auto program = detail::Program(s,
                                 detail::util::hash("//Map\n"
                                                    + C<Tin>::deviceFunctions()
                                                    + _source) );

  // modify program
  if (!program.loadBinary()) {
    // append parameters from user function to kernel
    program.transferParameters(_funcName, 1, "SCL_MAP");
    program.transferArguments(_funcName, 1, "SCL_FUNC");
    // rename user function
    program.renameFunction(_funcName, "SCL_FUNC");
    // rename typedefs
    program.adjustTypes<Tin, Tout>();
  }

  // build program
  program.build();

  return program;
}


// ## Map<Tin, void> ################################################
template<typename Tin>
Map<void(Tin)>::Map(const Source& source,
                    const std::string& funcName)
  : Skeleton(),
    _source(source),
    _funcName(funcName)
{
}

template <typename Tin>
template <template <typename> class C,
          typename... Args>
void Map<void(Tin)>::operator()(const C<Tin>& input,
                                Args&&... args)
{
  // create program dependent on the type of the input
  auto program = createAndBuildProgram<C>();

  this->prepareInput(input);

  prepareAdditionalInput(std::forward<Args>(args)...);

  execute(program, input, std::forward<Args>(args)...);

  updateModifiedStatus(std::forward<Args>(args)...);
}

template <typename Tin>
template <template <typename> class C,
          typename... Args>
void Map<void(Tin)>::execute(const detail::Program& program,
                             const C<Tin>& input,
                             Args&&... args)
{
  for (auto& devicePtr : input.distribution().devices()) {
    auto& inputBuffer  = input.deviceBuffer(*devicePtr);

    cl_uint elements   = inputBuffer.size();
    cl_uint local      = std::min(this->workGroupSize(),
                                  devicePtr->maxWorkGroupSize());
    cl_uint global     = detail::util::ceilToMultipleOf(elements, local);

    try {
      cl::Kernel kernel(program.kernel(*devicePtr, "SCL_MAP"));

      kernel.setArg(0, inputBuffer.clBuffer());
      kernel.setArg(1, elements);

      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 2,
                                        std::forward<Args>(args)...);

      // after finishing the kernel invoke this function ...
      auto invokeAfter =  [&] () {
                            inputBuffer.markAsNotInUse();
                          };

      devicePtr->enqueue(kernel,
                         cl::NDRange(global), cl::NDRange(local),
                         cl::NullRange, // offset
                         invokeAfter);

      // after successfully enqueued the kernel ...
      inputBuffer.markAsInUse();
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
  }
  LOG_INFO("Map kernel started");
}

template <typename Tin>
template <template <typename> class C>
detail::Program Map<void(Tin)>::createAndBuildProgram() const
{
  ASSERT_MESSAGE(!_source.empty(),
    "Tried to create program with empty user source.");

  // create program

  std::string s(C<Tin>::deviceFunctions());
  // first: user defined source
  s.append(_source);
  // second: append skeleton implementation source
  s.append(R"(

typedef float SCL_TYPE_0;

__kernel void SCL_MAP(
    const __global SCL_TYPE_0*  SCL_IN,
    const unsigned int          SCL_ELEMENTS )
{
  if (get_global_id(0) < SCL_ELEMENTS) {
    SCL_FUNC(SCL_IN[get_global_id(0)]);
  }
}
)");
  auto program = detail::Program(s,
                                 detail::util::hash("//Map\n"
                                                    + C<Tin>::deviceFunctions()
                                                    + _source) );

  // modify program
  if (!program.loadBinary()) {
    // append parameters from user function to kernel
    program.transferParameters(_funcName, 1, "SCL_MAP");
    program.transferArguments(_funcName, 1, "SCL_FUNC");
    // rename user function
    program.renameFunction(_funcName, "SCL_FUNC");
  // rename typedefs
    program.adjustTypes<Tin>();
  }

  // build program
  program.build();

  return program;
}

} // namespace skelcl

#endif // MAP_DEF_H_
