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

#include <cmath>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#undef  __CL_ENABLE_EXCEPTIONS

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>

#include "../Distributions.h"
#include "../Index.h"
#include "../Out.h"
#include "../Matrix.h"
#include "../Source.h"
#include "../Vector.h"

#include "Device.h"
#include "KernelUtil.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

namespace skelcl {

template<typename Tin, typename Tout>
Map<Tout(Tin)>::Map(const Source& source,
                    const std::string& funcName)
  : Skeleton(),
    detail::MapHelper<Tout(Tin)>(createAndBuildProgram(source, funcName))
{
  LOG_DEBUG_INFO("Create new Map object (", this, ")");
}

template <typename Tin, typename Tout>
template <template <typename> class C,
          typename... Args>
C<Tout> Map<Tout(Tin)>::operator()(const C<Tin>& input,
                                   Args&&... args) const
{
  C<Tout> output;
  this->operator()(out(output), input, std::forward<Args>(args)...);
  return output;
}

template <typename Tin, typename Tout>
template <template <typename> class C,
          typename... Args>
C<Tout>& Map<Tout(Tin)>::operator()(Out<C<Tout>> output,
                                    const C<Tin>& input,
                                    Args&&... args) const
{
LOG_INFO("start prepareInput");
  this->prepareInput(input);

LOG_INFO("start prepareAdditionalInput");
  prepareAdditionalInput(std::forward<Args>(args)...);

LOG_INFO("start prepareOutput");
  this->prepareOutput(output.container(), input);

LOG_INFO("start execute");
  execute(output.container(), input, std::forward<Args>(args)...);

LOG_INFO("start updateModifiedStatus");
  updateModifiedStatus(output, std::forward<Args>(args)...);

LOG_INFO("start return");
  return output.container();
}

template <typename Tin, typename Tout>
template <template <typename> class C,
          typename... Args>
void Map<Tout(Tin)>::execute(C<Tout>& output,
                             const C<Tin>& input,
                             Args&&... args) const
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
      cl::Kernel kernel(this->_program.kernel(*devicePtr, "SCL_MAP"));

      kernel.setArg(0, inputBuffer.clBuffer());
      kernel.setArg(1, outputBuffer.clBuffer());
      kernel.setArg(2, elements);

      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 3,
                                        std::forward<Args>(args)...);

      auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                                                     inputBuffer.clBuffer(),
                                                     outputBuffer.clBuffer(),
                                                     std::forward<Args>(args)...
                                                    );

      // after finishing the kernel invoke this function ...
      auto invokeAfter =  [=] () {
                                    (void)keepAlive;
                                 };

      LOG_INFO("Map kernel started with: ", local, " local WIs and ", global, " global WIs");
      auto event = devicePtr->enqueue(kernel,
                         cl::NDRange(global), cl::NDRange(local),
                         cl::NullRange, // offset
                         invokeAfter);
      event.wait();

						cl_int err = 0; 
						auto start = event.template getProfilingInfo<CL_PROFILING_COMMAND_START>(&err); 
						auto end = event.template getProfilingInfo<CL_PROFILING_COMMAND_END>(&err);
						LOG_INFO("kernel execution: ", (end - start) / 1e6, "ms");
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
  }
  LOG_DEBUG_INFO("Map kernel started");
}

template <typename Tin, typename Tout>
detail::Program
  Map<Tout(Tin)>::createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const
{
  ASSERT_MESSAGE(!source.empty(),
    "Tried to create program with empty user source.");

  // create program
  // first: device specific functions
  std::string s(detail::CommonDefinitions::getSource());
  // second: user defined source
  s.append(source);
  // last: append skeleton implementation source
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
  auto program = detail::Program(s, detail::util::hash(s));

  // modify program
  if (!program.loadBinary()) {
    // append parameters from user function to kernel
    program.transferParameters(funcName, 1, "SCL_MAP");
    program.transferArguments(funcName, 1, "SCL_FUNC");
    // rename user function
    program.renameFunction(funcName, "SCL_FUNC");
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
    detail::MapHelper<void(Tin)>(createAndBuildProgram(source, funcName))
{
}

template <typename Tin>
template <template <typename> class C,
          typename... Args>
void Map<void(Tin)>::operator()(const C<Tin>& input,
                                Args&&... args) const
{
  this->prepareInput(input);

  prepareAdditionalInput(std::forward<Args>(args)...);

  execute(input, std::forward<Args>(args)...);

  updateModifiedStatus(std::forward<Args>(args)...);
}

template <typename Tin>
template <template <typename> class C,
          typename... Args>
void Map<void(Tin)>::execute(const C<Tin>& input,
                             Args&&... args) const
{
  for (auto& devicePtr : input.distribution().devices()) {
    auto& inputBuffer  = input.deviceBuffer(*devicePtr);

    cl_uint elements   = inputBuffer.size();
    cl_uint local      = std::min(this->workGroupSize(),
                                  devicePtr->maxWorkGroupSize());
    cl_uint global     = detail::util::ceilToMultipleOf(elements, local);

    try {
      cl::Kernel kernel(this->_program.kernel(*devicePtr, "SCL_MAP"));

      kernel.setArg(0, inputBuffer.clBuffer());
      kernel.setArg(1, elements);

      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 2,
                                        std::forward<Args>(args)...);

      auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                                                     inputBuffer.clBuffer(),
                                                     std::forward<Args>(args)...
                                                    );

      // after finishing the kernel invoke this function and release keepAlive
      auto invokeAfter =  [=] () {
                                    (void)keepAlive;
                                 };

      devicePtr->enqueue(kernel,
                         cl::NDRange(global), cl::NDRange(local),
                         cl::NullRange, // offset
                         invokeAfter);
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
  }
  LOG_DEBUG_INFO("Map kernel started");
}

template <typename Tin>
detail::Program
  Map<void(Tin)>::createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const
{
  ASSERT_MESSAGE(!source.empty(),
    "Tried to create program with empty user source.");

  // create program
  // first: device specific functions
  std::string s(detail::CommonDefinitions::getSource());
  // second: user defined source
  s.append(source);
  // last: append skeleton implementation source
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
  auto program = detail::Program(s, detail::util::hash(s));

  // modify program
  if (!program.loadBinary()) {
    // append parameters from user function to kernel
    program.transferParameters(funcName, 1, "SCL_MAP");
    program.transferArguments(funcName, 1, "SCL_FUNC");
    // rename user function
    program.renameFunction(funcName, "SCL_FUNC");
  // rename typedefs
    program.adjustTypes<Tin>();
  }

  // build program
  program.build();

  return program;
}

// ## Map<Index, Tout> ################################################
template <typename Tout>
Map<Tout(Index)>::Map(const Source& source,
                      const std::string& funcName)
  : Skeleton(),
    detail::MapHelper<Tout(Index)>(createAndBuildProgram(source, funcName))
{
}

template <typename Tout>
template <typename... Args>
Vector<Tout> Map<Tout(Index)>::operator()(const Vector<Index>& input,
                                          Args&&... args) const
{
  Vector<Tout> output;
  this->operator()(out(output), input, std::forward<Args>(args)...);
  return output;
}

template <typename Tout>
template <typename... Args>
Vector<Tout>& Map<Tout(Index)>::operator()(Out<Vector<Tout>> output,
                                           const Vector<Index>& input,
                                           Args&&... args) const
{
  // set default distribution if required
  if (!input.distribution().isValid()) {
    input.setDistribution(detail::BlockDistribution<Vector<Index>>());
  }
  // no need to fully prepare index container

  prepareAdditionalInput(std::forward<Args>(args)...);

  this->prepareOutput(output.container(), input);

  execute(output.container(), input, std::forward<Args>(args)...);

  updateModifiedStatus(output, std::forward<Args>(args)...);

  return output.container();
}

template <typename Tout>
template <typename... Args>
void Map<Tout(Index)>::execute(Vector<Tout>& output,
                               const Vector<Index>& input,
                               Args&&... args) const
{
  ASSERT( output.size() >= input.size() );

  auto sizes = input.sizes();
  size_t i = 0;
  cl_uint offset = 0;
  for (auto& devicePtr : input.distribution().devices()) {
    auto& outputBuffer = output.deviceBuffer(*devicePtr);

    cl_uint local      = std::min(this->workGroupSize(),
                                  devicePtr->maxWorkGroupSize());
    cl_uint global     = detail::util::ceilToMultipleOf(sizes[i], local);

    try {
      cl::Kernel kernel(this->_program.kernel(*devicePtr, "SCL_MAP"));

      kernel.setArg(0, outputBuffer.clBuffer());
      kernel.setArg(1, offset);

      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 2,
                                        std::forward<Args>(args)...);

      auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                                                     std::forward<Args>(args)...);

      // after finishing the kernel invoke this function ...
      auto invokeAfter =  [=] () {
                                    (void)keepAlive;
                                 };

      devicePtr->enqueue(kernel,
                         cl::NDRange(global), cl::NDRange(local),
                         cl::NullRange,
                         invokeAfter);
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
    offset += sizes[i];
    ++i;
  }
  LOG_DEBUG_INFO("Map kernel started");
}

template <typename Tout>
detail::Program
  Map<Tout(Index)>::createAndBuildProgram(const std::string& source,
                                          const std::string& funcName) const
{
  ASSERT_MESSAGE(!source.empty(),
    "Tried to create program with empty user source.");

  // create program
  // first: device specific functions
  std::string s(detail::CommonDefinitions::getSource());
  s.append(R"(
typedef size_t Index;

)");
  // second: user defined source
  s.append(source);
  // last: append skeleton implementation source
  s.append(R"(

typedef float SCL_TYPE_0;

__kernel void SCL_MAP(
          __global SCL_TYPE_0*  SCL_OUT,
    const unsigned int          SCL_OFFSET)
{
  SCL_OUT[get_global_id(0)] = SCL_FUNC(get_global_id(0)+SCL_OFFSET);
}
)");
  auto program = detail::Program(s, detail::util::hash(s));

  // modify program
  if (!program.loadBinary()) {
    // append parameters from user function to kernel
    program.transferParameters(funcName, 1, "SCL_MAP");
    program.transferArguments(funcName, 1, "SCL_FUNC");
    // rename user function
    program.renameFunction(funcName, "SCL_FUNC");
    // rename typedefs
    program.adjustTypes<Tout>();
  }

  // build program
  program.build();

  return program;
}

// ## Map<Index, void> ################################################
// constructor definition in Map.cpp to avoid problems during linking

template <typename... Args>
void Map<void(Index)>::operator()(const Vector<Index>& input,
                                  Args&&... args) const
{
  // set default distribution if required
  if (!input.distribution().isValid()) {
    input.setDistribution(detail::BlockDistribution<Vector<Index>>());
  }
  // no need to further prepare index container

  prepareAdditionalInput(std::forward<Args>(args)...);

  execute(input, std::forward<Args>(args)...);

  updateModifiedStatus(std::forward<Args>(args)...);
}

template <typename... Args>
void Map<void(Index)>::execute(const Vector<Index>& input,
                               Args&&... args) const
{
  auto sizes = input.sizes();
  size_t i = 0;
  cl_uint offset = 0;
  for (auto& devicePtr : input.distribution().devices()) {

    cl_uint local      = std::min(this->workGroupSize(),
                                  devicePtr->maxWorkGroupSize());
    cl_uint global     = detail::util::ceilToMultipleOf(sizes[i], local);

    try {
      cl::Kernel kernel(this->_program.kernel(*devicePtr, "SCL_MAP"));

      kernel.setArg(0, offset);

      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 1,
                                        std::forward<Args>(args)...);

      auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                                                     std::forward<Args>(args)...
                                                    );

      // after finishing the kernel invoke this function ...
      auto invokeAfter =  [=] () { (void)keepAlive; };

      devicePtr->enqueue(kernel,
                         cl::NDRange(global), cl::NDRange(local),
                         cl::NullRange,
                         invokeAfter);
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
    offset += sizes[i];
    ++i;
  }
  LOG_DEBUG_INFO("Map kernel started");
}


// ## Map<IndexPoint, Tout> ################################################
template <typename Tout>
Map<Tout(IndexPoint)>::Map(const Source& source,
                           const std::string& funcName)
: Skeleton(),
  detail::MapHelper<Tout(IndexPoint)>(createAndBuildProgram(source, funcName))
{
}

template <typename Tout>
template <typename... Args>
Matrix<Tout> Map<Tout(IndexPoint)>::operator()(const Matrix<IndexPoint>& input,
                                               Args&&... args) const
{
  Matrix<Tout> output;
  this->operator()(out(output), input, std::forward<Args>(args)...);
  return output;
}

template <typename Tout>
template <typename... Args>
Matrix<Tout>& Map<Tout(IndexPoint)>::operator()(Out<Matrix<Tout>> output,
                                                const Matrix<IndexPoint>& input,
                                                Args&&... args) const
{
  // set default distribution if required
  if (!input.distribution().isValid()) {
    input.setDistribution(detail::BlockDistribution<Matrix<Index>>());
  }
  // no need to further prepare index matrix
  
  prepareAdditionalInput(std::forward<Args>(args)...);
  
  this->prepareOutput(output.container(), input);
  
  execute(output.container(), input, std::forward<Args>(args)...);
  
  updateModifiedStatus(output, std::forward<Args>(args)...);
  
  return output.container();
}

template <typename Tout>
template <typename... Args>
void Map<Tout(IndexPoint)>::execute(Matrix<Tout>& output,
                                    const Matrix<IndexPoint>& input,
                                    Args&&... args) const
{
  ASSERT( output.size() >= input.size() );
  
  size_t i = 0;
  cl_uint rowOffset = 0;
  for (auto& devicePtr : input.distribution().devices()) {
    auto& outputBuffer = output.deviceBuffer(*devicePtr);
    
    auto elements = input.distribution().sizeForDevice(input, devicePtr);
    cl_uint colCount = input.size().columnCount();
    cl_uint rowCount = elements / colCount;
    
    size_t wgSize      = detail::util::floorPow2( sqrt(this->workGroupSize()) );
    cl_uint local      = std::min(wgSize,
                                  devicePtr->maxWorkGroupSize());
    cl_uint colGlobal  = detail::util::ceilToMultipleOf(colCount, local);
    cl_uint rowGlobal  = detail::util::ceilToMultipleOf(rowCount, local);
    
    try {
      cl::Kernel kernel(this->_program.kernel(*devicePtr, "SCL_MAP"));
      
      kernel.setArg(0, outputBuffer.clBuffer());
      kernel.setArg(1, rowOffset);
      kernel.setArg(2, colCount);
      
      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 3,
                                        std::forward<Args>(args)...);
      
      auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                                                     outputBuffer.clBuffer(),
                                                     std::forward<Args>(args)...
                                                    );
      
      // after finishing the kernel invoke this function ...
      auto invokeAfter =  [=] () { (void)keepAlive; };
      
      devicePtr->enqueue(kernel,
                         cl::NDRange(rowGlobal, colGlobal),
                         cl::NDRange(local, local),
                         cl::NullRange,
                         invokeAfter);
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
    rowOffset += rowCount;
    ++i;
  }
  LOG_DEBUG_INFO("Map kernel started");
}

template <typename Tout>
detail::Program
  Map<Tout(IndexPoint)>::createAndBuildProgram(
                                               const std::string& source,
                                               const std::string& funcName
                                              ) const
{
  ASSERT_MESSAGE(!source.empty(),
                 "Tried to create program with empty user source.");
  
  // create program
  // first: device specific functions
  std::string s(detail::CommonDefinitions::getSource());
  s.append(R"(
typedef struct {
  size_t x;
  size_t y;
} IndexPoint;
           
           )");
  // second: user defined source
  s.append(source);
  // last: append skeleton implementation source
  s.append(R"(
           
           typedef float SCL_TYPE_0;
           
           __kernel void SCL_MAP(__global SCL_TYPE_0*  SCL_OUT,
                                 const unsigned int          SCL_ROW_OFFSET,
                                 const unsigned int          SCL_COL_COUNT)
  {
    // dim 1 is the columns, dim 0 the rows
    IndexPoint p;
    p.x = get_global_id(1);
    p.y = get_global_id(0) + SCL_ROW_OFFSET;
    SCL_OUT[get_global_id(0) * SCL_COL_COUNT + get_global_id(1)] = SCL_FUNC(p);
  }
           )");
  auto program = detail::Program(s, detail::util::hash(s));
  
  // modify program
  if (!program.loadBinary()) {
    // append parameters from user function to kernel
    program.transferParameters(funcName, 1, "SCL_MAP");
    program.transferArguments(funcName, 1, "SCL_FUNC");
    // rename user function
    program.renameFunction(funcName, "SCL_FUNC");
    // rename typedefs
    program.adjustTypes<Tout>();
  }
  
  // build program
  program.build();
  
  return program;
}

// ## Map<IndexPoint, void> ################################################
// constructor definition in Map.cpp to avoid problems during linking

template <typename... Args>
void Map<void(IndexPoint)>::operator()(const Matrix<IndexPoint>& input,
                                       Args&&... args) const
{
  // set default distribution if required
  if (!input.distribution().isValid()) {
    input.setDistribution(detail::BlockDistribution<Matrix<IndexPoint>>());
  }
  // no need to further prepare index container
  
  prepareAdditionalInput(std::forward<Args>(args)...);
  
  execute(input, std::forward<Args>(args)...);
  
  updateModifiedStatus(std::forward<Args>(args)...);
}

template <typename... Args>
void Map<void(IndexPoint)>::execute(const Matrix<IndexPoint>& input,
                                    Args&&... args) const
{
  size_t i = 0;
  cl_uint rowOffset = 0;
  for (auto& devicePtr : input.distribution().devices()) {
    auto elements = input.distribution().sizeForDevice(input, devicePtr);
    auto colCount = input.size().columnCount();
    auto rowCount = elements / colCount;
    
    size_t wgSize      =  detail::util::floorPow2(sqrt(this->workGroupSize()));
    cl_uint local      = std::min(wgSize,
                                  devicePtr->maxWorkGroupSize());
    
    cl_uint colGlobal  = detail::util::ceilToMultipleOf(colCount, local);
    cl_uint rowGlobal  = detail::util::ceilToMultipleOf(rowCount, local);
    
    try {
      cl::Kernel kernel(this->_program.kernel(*devicePtr, "SCL_MAP"));
      
      kernel.setArg(0, rowOffset);
      
      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 1,
                                        std::forward<Args>(args)...);
      
      auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                                                     std::forward<Args>(args)...
                                                    );
      
      // after finishing the kernel invoke this function ...
      auto invokeAfter =  [=] () {
        (void)keepAlive;
      };
      
      devicePtr->enqueue(kernel,
                         cl::NDRange(rowGlobal, colGlobal),
                         cl::NDRange(local, local),
                         cl::NullRange,
                         invokeAfter);
    } catch (cl::Error& err) {
      ABORT_WITH_ERROR(err);
    }
    rowOffset += rowCount;
    ++i;
  }
  LOG_DEBUG_INFO("Map kernel started");
}
  
} // namespace skelcl

#endif // MAP_DEF_H_
