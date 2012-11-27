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
/// \file ReduceDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef REDUCE_DEF_H_
#define REDUCE_DEF_H_

#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#undef  __CL_ENABLE_EXCEPTIONS

#include "../Distributions.h"
#include "../Out.h"
#include "../Source.h"

#include "Assert.h"
#include "Device.h"
#include "DeviceBuffer.h"
#include "DeviceList.h"
#include "KernelUtil.h"
#include "Logger.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"


namespace skelcl {

template <typename T>
Reduce<T(T)>::Reduce(const Source& source,
                     const std::string& id,
                     const std::string& funcName)
  : detail::Skeleton(), _id(id), _funcName(funcName), _userSource(source)
{
}


template <typename T>
template <typename... Args>
Vector<T> Reduce<T(T)>::operator()(const Vector<T>& input,
                                   Args&&... args)
{
  Vector<T> output;
  this->operator()(out(output), input, std::forward<Args>(args)...);
  return output;
}


template <typename T>
template <typename... Args>
Vector<T>& Reduce<T(T)>::operator()(Out< Vector<T> > output,
                                    const Vector<T>& input,
                                    Args&&... args)
{
  // first prepare input to set the default distribution if needed ...
  prepareInput(input);

  // TODO: relax to multiple devices later
  ASSERT(input.distribution().devices().size() == 1);

  auto& device = *(input.distribution().devices().front());

  // ... determine if and how the first reduction step is performed ...
  auto firstLevel  = determineFirstLevelParameters(device, input, output.container());
  if (firstLevel) {
    LOG_DEBUG_INFO("firstLevel.workGroupSize: ",    firstLevel->workGroupSize,
                   ", firstLevel.workGroupCount: ", firstLevel->workGroupCount);
  }

  // ... determine how the second (and final) reduction step is performed ...
  auto secondLevel = determineSecondLevelParameters(device, input, output.container(), firstLevel);
  LOG_DEBUG_INFO("secondLevel.workGroupSize: ",    secondLevel->workGroupSize,
                 ", secondLevel.workGroupCount: ", secondLevel->workGroupCount);

  // ... prepare output ...
  if (firstLevel) {
    // if a first reduction step is used, the output also serves
    // as temporary vector
    prepareOutput(output.container(), input, firstLevel->workGroupSize);
  } else { // no first level
    prepareOutput(output.container(), input, 1);
  }

  // ... execute first reduction step ...
  if (firstLevel) {
    execute(device, firstLevel, std::forward<Args>(args)...);
  }

  // ... execute second (final) reduction step ...
  execute(device, secondLevel, std::forward<Args>(args)...);

  // ... finally update modification status.
  updateModifiedStatus(output, std::forward<Args>(args)...);

  return output.container();
}


// private member functions

template <typename T>
void Reduce<T(T)>::prepareInput(const Vector<T>& input)
{
  // set default distribution if required
  if (!input.distribution().isValid()) {
    input.setDistribution(detail::SingleDistribution< Vector<T> >());
  }
  // create buffers if required
  input.createDeviceBuffers();
  // copy data to devices
  input.startUpload();
}


template <typename T>
void Reduce<T(T)>::prepareOutput(Vector<T>& output,
                                 const Vector<T>& input,
                                 const size_t size)
{
  ASSERT(size > 0);
  if (static_cast<const void*>(&output) == static_cast<const void*>(&input)) {
    return; // already prepared in prepareInput
  }
  // resize container if required
  if (output.size() < size) {
    output.resize(size);
  }
  // set output distribution
  output.setDistribution(input.distribution());
  // create buffers if required
  output.createDeviceBuffers();
}


template <typename T>
template <typename... Args>
void Reduce<T(T)>::execute(const detail::Device& device,
                           const std::shared_ptr<Level>& level,
                           Args&&... args)
{
  ASSERT( level->inputPtr->distribution().isValid() );

  auto& outputBuffer= level->outputPtr->deviceBuffer(device);
  auto& inputBuffer = level->inputPtr->deviceBuffer(device);

  cl_uint elements  = level->inputPtr->size();
  cl_uint local     = level->workGroupSize;
  cl_uint global    = level->workGroupSize * level->workGroupCount;

  try {
    cl::Kernel kernel(level->program->kernel(device, "SCL_REDUCE"));

    kernel.setArg(0, inputBuffer.clBuffer());
    kernel.setArg(1, outputBuffer.clBuffer());
    // allocate shared memory size
    kernel.setArg(2, cl::__local( sizeof(T) * local ) );
    kernel.setArg(3, elements);

    detail::kernelUtil::setKernelArgs(kernel, device, 4,
                                      std::forward<Args>(args)...);

    auto keepAlive = detail::kernelUtil::keepAlive(device,
                                                   inputBuffer.clBuffer(),
                                                   outputBuffer.clBuffer(),
                                                   std::forward<Args>(args)...);

    // after finishing the kernel invoke this function ...
    auto invokeAfter =  [=] () {
                                  (void)keepAlive;
                               };

    device.enqueue(kernel,
                   cl::NDRange(global), cl::NDRange(local),
                   cl::NullRange, // offset
                   invokeAfter);
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }
}


template <typename T>
size_t Reduce<T(T)>::determineWorkGroupSize(const detail::Device& device)
{
  unsigned long maxWorkGroupSize = std::min( device.maxWorkGroupSize(),
                                             static_cast<size_t>(64) );
  unsigned long maxSizeToFitInLocalMem = device.localMemSize() / sizeof(T);

  return std::min(maxWorkGroupSize, maxSizeToFitInLocalMem);
}


template <typename T>
size_t
  Reduce<T(T)>::determineFirstLevelWorkGroupCount(const detail::Device& device,
                                                  const Vector<T>& input,
                                                  const size_t workGroupSize)
{
  auto inputSize          = input.deviceBuffer(device).size();
  auto workGroupStepWidth = 2 * workGroupSize;
  if (inputSize <= workGroupStepWidth) {
    return 0; // no first level necessary
  } else {
    // max so many work groups, that the results can be handled
    // in a single step
    auto maxWorkGroupCount = workGroupStepWidth;
    auto count =   (inputSize / workGroupStepWidth) // round up
                 + ( (inputSize % workGroupStepWidth != 0) ? 1 : 0 );
    auto workGroupCount = std::min(maxWorkGroupCount, count);
    return workGroupCount;
  }
}


template <typename T>
std::shared_ptr<typename Reduce<T(T)>::Level>
  Reduce<T(T)>::determineFirstLevelParameters(const detail::Device& device,
                                              const Vector<T>& input,
                                              const Vector<T>& output)
{
  std::shared_ptr<typename Reduce<T(T)>::Level> level;

  auto wgSize  = determineWorkGroupSize( device );
  auto wgCount = determineFirstLevelWorkGroupCount( device, input, wgSize );

  if (wgCount > 16) { // with fewer than 16 groups let second level handle it
    level = std::make_shared<typename Reduce<T(T)>::Level>();
    level->workGroupSize  = wgSize;
    level->workGroupCount = wgCount;
    level->inputPtr       = &input;
    level->outputPtr      = &output;

    std::stringstream preamble;
    preamble << "#define GROUP_SIZE (" << level->workGroupSize << ")\n"
             << "#define SCL_IDENTITY (" << _id << ")\n";
    level->program = createPrepareAndBuildProgram( preamble.str() );
  }

  return level;
}


template <typename T>
std::shared_ptr<typename Reduce<T(T)>::Level>
  Reduce<T(T)>::determineSecondLevelParameters(const detail::Device& device,
                                               const Vector<T>& input,
                                               const Vector<T>& output,
                                               const std::shared_ptr<Reduce<T(T)>::Level>& firstLevel)
{
  auto level = std::make_shared<typename Reduce<T(T)>::Level>();

  level->workGroupSize  = determineWorkGroupSize( device );
  level->workGroupCount = 1;
  if (firstLevel) {
    level->inputPtr     = &output;
  } else {
    level->inputPtr     = &input;
  }
  level->outputPtr      = &output;

  std::stringstream preamble;
  preamble << "#define GROUP_SIZE (" << level->workGroupSize << ")\n"
           << "#define SCL_IDENTITY (" << _id << ")\n";
  level->program = createPrepareAndBuildProgram( preamble.str() );

  return level;
}

template <typename T>
std::shared_ptr<skelcl::detail::Program>
  Reduce<T(T)>::createPrepareAndBuildProgram(const std::string& preamble)
{
  ASSERT_MESSAGE(!_userSource.empty(),
    "Tried to create program with empty user source.");
  // first: user defined source
  std::string s(preamble + _userSource);
  // second: append skeleton implementation source
  s.append(
    #include "ReduceKernel.cl"
  );

  auto program = std::make_shared<detail::Program>(s
        /*, skelcl::detail::util::hash("//Reduce\n"+source)*/);
  //if (!program->loadBinary()) {
    program->transferParameters(_funcName, 2, "SCL_REDUCE");
    program->transferArguments(_funcName, 2, "SCL_FUNC");
    program->renameFunction(_funcName, "SCL_FUNC");
    program->adjustTypes<T>();
  //}
  program->build();
  return program;
}

} // namespace skelcl

#endif // REDUCE_DEF_H_
