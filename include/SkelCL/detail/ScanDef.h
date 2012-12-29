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
/// \file ScanDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef SCAN_DEF_H_
#define SCAN_DEF_H_

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

template<typename T>
Scan<T(T)>::Scan(const Source& source,
                 const std::string& id,
                 const std::string& funcName)
  : detail::Skeleton(),
    _program(createAndBuildProgram(source, id, funcName))
{
  LOG_DEBUG_INFO("Create new Scan object (", this, ")");
}

template <typename T>
template <typename... Args>
Vector<T> Scan<T(T)>::operator()(const Vector<T>& input, Args&&... args)
{
  Vector<T> output;
  this->operator()(out(output), input, std::forward<Args>(args)...);
  return output;
}

template <typename T>
template <typename... Args>
Vector<T>& Scan<T(T)>::operator()(Out<Vector<T>> output,
                                  const Vector<T>& input,
                                  Args&&... args)
{
  prepareInput(input);

  prepareAdditionalInput(std::forward<Args>(args)...);

  prepareOutput(output.container(), input);

  execute(output.container(), input, std::forward<Args>(args)...);

  updateModifiedStatus(output, std::forward<Args>(args)...);

  return output.container();
}

template <typename T>
template <typename... Args>
void Scan<T(T)>::execute(Vector<T>& output,
                         const Vector<T>& input,
                         Args&&... args)
{
  ASSERT( input.distribution().isValid() );

  // TODO: relax to multiple devices later
  ASSERT( input.distribution().decies().size() == 1 );

  auto& device        = *(input.distribution().devices().front());
  auto& outputBuffer  = output.deviceBuffer(device);
  auto& inputBuffer   = input.deviceBuffer(device);

  cl_uint elements    = inputBuffer.size();
  cl_uint wgSize      = std::min(this->workGroupSize(),
                                 device.maxWorkGroupSize());

  // calculate number of passes
  unsigned int passes = calculateNumberOfPasses(wgSize, elements);

  // allocate intermediate buffers
  detail::DeviceBuffer tmpBuffers[passes];
  cl_uint n = elements;
  for (auto& buffer : tmpBuffers) {
    buffer = detail::DeviceBuffer(&device, n, sizeof(T));

    n = (n + wgSize - 1) / wgSize; // round up while dividing
  }

  cl::Kernel scanKernel(_program.kernel(device, "SCL_SCAN"));
  // allocate shared memory
  scanKernel.setArg( 1, cl::__local(sizeof(T) * wgSize) );

  // perform the scan for each pass
  auto* currentInput = &input.deviceBuffer(device);
  for (unsigned int i = 0; i < passes; i++) {
    auto* currentOutput = tmpBuffers[i];

    cl_uint local  = workGroupSize / 2;
    cl_uint global = detail::util::ceilToMultipleOf(currentOutput->size() / 2,
                                                    local);
    scanKernel.setArg(0, currentInput->clBuffer());
    scanKernel.setArg(2, currentOutput->clBuffer());
    scanKernel.setArg(3, currentOutput->size());

    // set additional kernel args

    // launch kernel
    device.enqueue(scanKernel, cl::NDRange(global), cl::NDRange(local));

    // use the calculated results as input for the next pass
    currentInput = currentOutput;
  }

  // perform uniform addition as last step of the scan
  cl::Kernel uniformAddKernel(_program.kernel(device, "SCL_UNIFORM_ADD"));
  for (unsigned int i = passes - 2; i >= 0; i--) {
    auto* currentInput = tmpBuffers[i];
    detail::DeviceBuffer* currentOutput = nullptr;
    if ( i > 0 ) {
      currentOutput = tmpBuffers[i-1];
    } else { // i == 0
      currentOutput = &outputBuffer;
    }

    cl_uint local = workGroupSize / 2;
    cl_uint global = detail::util::ceilToMultipleOf(currentInput->size() / 2,
                                                    local);
    uniformAddKernel.setArg(0, currentOutput->clBuffer());
    uniformAddKernel.setArg(1, currentInput->clBuffer());
    uniformAddKernel.setArg(2, currentInput->size());

    device.enqueue(uniformAddKernel, cl::NDRange(global), cl::NDRange(local));
  }
  LOG_DEBUG_INFO("Scan kernel started");
}

template <typename T>
unsigned int Scan<T(T)>::calculateNumberOfPasses(size_t workGroupSize,
                                                 size_t elements) const
{
  unsigned int passes = 0;
  do {
    // round up while dividing
    elements = (elements + workGroupSize - 1) / workGroupSize;
    passes++;
  } while (elements > 1);
  return passes
}

template<typename T>
detail::Program
  Scan<T(T)>::createAndBuildProgram(const std::string& source,
                                    const std::string& id,
                                    const std::string& funcName) const
{
  ASSERT_MESSAGE(!source.empty(),
    "Tried to create program with empty user source.");

  // create program
  // first: device specific functions
  std::string deviceFunctions;
  deviceFunctions.append(Vector<Tleft>::deviceFunctions());
  deviceFunctions.append(Matrix<Tleft>::deviceFunctions());
  
  std::string s(deviceFunctions);
  // second: user defined source
  s.append(source);
  // last: append skeleton implementation source
  s.append(
    #include "ScanKernel.cl"
  );
  auto program = detail::Program(s,
                                 detail::util::hash("//Scan\n"
                                                    + deviceFunctions
                                                    + source) );

  // modify program
  if (!program.loadBinary()) {
#if 0
    // append parameters from user function to kernel
    program.transferParameters(funcName, 2, "SCL_ZIP");
    program.transferArguments(funcName, 2, "SCL_FUNC");
    // rename user function
    program.renameFunction(funcName, "SCL_FUNC");
#endif
    // rename typedefs
    program.adjustTypes<T>();
  }
  // build program
  program.build();

  return program;
}

template <typename T>
void Scan<T(T)>::prepareInput(const Vector<T>& input)
{
  // set default distribution if required
  if (!input.distribution().isValid()) {
    input.setDistribution(detail::SinglDistribution<Vector<T>>());
  }
  // create buffers if required
  input.createDeviceBuffers();
  // copy data to devices
  input.startUpload();
}

template <typename T>
void Scan<T(T)>::prepareOutput(Vector<T>& output,
                               const Vector<T>& input)
{
  if (   static_cast<void*>(&output) == static_cast<const void*>(&input)) {
    return; // already prepared in prepareInput
  }
  // resize container if required
  if (output.size() < input.size()) {
    output.resize(input.size());
  }
  // adopt distribution from left input
  output.setDistribution(input.distribution());
  // create buffers if required
  output.createDeviceBuffers();
}

} // namespace skelcl

#endif // SCAN_DEF_H_

