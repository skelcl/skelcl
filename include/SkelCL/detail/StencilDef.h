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
/// \file StencilDef.h
///
/// Stencil template class implementation.
///
/// \author Stefan Breuer <s_breu03@uni-muenster.de>
/// \author Chris Cummins <chrisc.101@gmail.com>
///
#ifndef STENCILDEF_H_
#define STENCILDEF_H_

#include <algorithm>
#include <istream>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#undef __CL_ENABLE_EXCEPTIONS

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>
#include <pvsutil/Timer.h>
#include <SkelCL/StencilSequence.h>

#include "../Distributions.h"
#include "../Matrix.h"
#include "../Out.h"

#include "Device.h"
#include "KernelUtil.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

// #include "./OmniTuneClient.h"

// Use OmniTune to suggest workgroup sizes.
#define USE_OMNITUNE_WG 0

namespace skelcl {

template <typename Tin, typename Tout>
Stencil<Tout(Tin)>::Stencil(const Source& source,
                            const std::string& func,
                            const StencilShape& shape,
                            Padding padding,
                            Tin paddingElement) :
  detail::Skeleton("Stencil<Tout(Tin)>"), _shape(shape), _padding(padding),
  _paddingElement(paddingElement), _userSource(source), _funcName(func),
  _program(createAndBuildProgram())
{
    LOG_DEBUG_INFO("Created new Stencil object (", this, ")");
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout> Stencil<Tout(Tin)>::operator()(const Matrix<Tin>& input,
                                            Args&&... args) const
{
  Matrix<Tout> output;
  this->operator()(out(output), input, std::forward<Args>(args)...);
  return output;
}


template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout>& Stencil<Tout(Tin)>::operator()(Out<Matrix<Tout>> output,
                                             const Matrix<Tin>& input,
                                             Args&&... args) const
{
  ASSERT(input.rowCount() > 0);
  ASSERT(input.columnCount() > 0);

  execute(input, output.container(), input, 1, 0, _shape.getSouth(),
          _shape.getNorth(), std::forward<Args>(args)...);

  return output.container();
}

template <typename Tin, typename Tout>
template <typename T, typename... Args>
void Stencil<Tout(Tin)>::execute(const Matrix<Tin>& input,
                                 Matrix<Tout>& output,
                                 const Matrix<T>& initial,
                                 const unsigned int opsUntilNextSync,
                                 const unsigned int opsSinceLastSync,
                                 const unsigned int sumSouthBorders,
                                 const unsigned int sumNorthBorders,
                                 Args&&... args) const
{
  const size_t numDevices = input.distribution().devices().size();

  pvsutil::Timer t; // Time how long it takes to prepare input and output data.

  prepareInput(input);
  prepareAdditionalInput(std::forward<Args>(args)...);
  prepareOutput(output, input);

  // Profiling information.
  LOG_PROF(_name, "[", this, "] prepare ", t.stop(), " ms");

  for (auto& devicePtr : input.distribution().devices()) {
    cl_uint local[2], global[3];

    cl::Kernel kernel(_program.kernel(*devicePtr, "SCL_STENCIL"));

    // omnitune::stencil::getLocalSize(kernel, devicePtr->clDevice(),
                                    // _program.getCode(), &local[0]);
    local[0] = 256; // default wg size
    getGlobalSize(&local[0], output, devicePtr->id(), numDevices,
                  opsUntilNextSync, opsSinceLastSync, sumSouthBorders,
                  sumNorthBorders, &global[0]);
    setKernelArgs(output, input, initial, local, devicePtr,
                  kernel, std::forward<Args>(args)...);

    // Keep buffers and arguments alive / mark them as in use.
    auto keepAlive = detail::kernelUtil::keepAlive(
        *devicePtr,
        input.deviceBuffer(*devicePtr).clBuffer(),
        output.deviceBuffer(*devicePtr).clBuffer(),
        std::forward<Args>(args)...);
    auto invokeAfter = [=]() { (void)keepAlive; };

    // Each OpenCL event is identified using it's index in the private
    // _events vector. The current size of the vector is equivalent to
    // the index of the *next* element to be pushed to it.
    auto eventnum = _events.size();
    _events.push_back(devicePtr->enqueue(kernel,
                                         cl::NDRange(global[0], global[1]),
                                         cl::NDRange(local[0], local[1]),
                                         cl::NDRange(0, global[2]),
                                         invokeAfter));

    // Print global, local, and tile sizes.
    LOG_PROF(_name, "[", this, "][", eventnum,
             "] global[", global[0], "][", global[1],
             "] local[", local[0], "][", local[1]);
  }

  updateModifiedStatus(out(output), std::forward<Args>(args)...);
}

template <typename Tin, typename Tout>
void Stencil<Tout(Tin)>::getTileSize(const cl_uint *const local,
                                     cl_uint *const tile) const
{
  tile[0] = local[0] + _shape.getNorth() + _shape.getSouth();
  tile[1] = local[1] + _shape.getEast() + _shape.getWest();
}

template <typename Tin, typename Tout>
void Stencil<Tout(Tin)>::getGlobalSize(const cl_uint *const local,
                                       const Matrix<Tout>& output,
                                       const size_t deviceId,
                                       const size_t numDevices,
                                       const unsigned int opsUntilNextSync,
                                       const unsigned int opsSinceLastSync,
                                       const unsigned int sumSouthBorders,
                                       const unsigned int sumNorthBorders,
                                       cl_uint *const global) const
{
  cl_uint grid[2];

  // Determine the size of the grid being operated on, and the offset
  // into it.
  grid[0] = output.columnCount();
  if (numDevices == 1) {
    // For a single device, the grid size == global size
    grid[1] = output.rowCount();
    global[2] = 0;
  } else if (deviceId == 0) {
    grid[1] = output.rowCount() + opsUntilNextSync * sumSouthBorders;
    global[2] = 0;
  } else if (deviceId == numDevices - 1) {
    grid[1] = output.rowCount() + opsUntilNextSync * sumNorthBorders;
    global[2] = opsSinceLastSync * sumNorthBorders;
  } else {
    grid[1] = output.rowCount() + opsUntilNextSync * sumNorthBorders
              + opsUntilNextSync * sumSouthBorders;
    global[2] = opsSinceLastSync * sumNorthBorders;
  }

  // Global size must be a multiple of working group size in each
  // dimension.
  global[0] = detail::util::ceilToMultipleOf(grid[0], local[0]);
  global[1] = detail::util::ceilToMultipleOf(grid[1], local[1]);
}

template <typename Tin, typename Tout>
template <typename... Args>
void Stencil<Tout(Tin)>::
  setKernelArgs(Matrix<Tout>& output,
                const Matrix<Tin>& input,
                const Matrix<Tin>& initial,
                const cl_uint *const localSize,
                const std::shared_ptr<detail::Device>& devicePtr,
                cl::Kernel& kernel,
                Args&&... args) const
{
  cl_uint tile[2];

  auto& inputBuffer = input.deviceBuffer(*devicePtr);
  auto& outputBuffer = output.deviceBuffer(*devicePtr);
  auto& initialBuffer = initial.deviceBuffer(*devicePtr);

  getTileSize(&localSize[0], &tile[0]);

  const cl_uint clNumElements = static_cast<cl_uint>(inputBuffer.size());
  const cl_uint clColumnCount = static_cast<cl_uint>(output.columnCount());
  const cl_uint tileSizeBytes = tile[0] * tile[1] * sizeof(Tin);

  int j = 0;
  kernel.setArg(j++, inputBuffer.clBuffer());    // SCL_IN
  kernel.setArg(j++, outputBuffer.clBuffer());   // SCL_OUT
  kernel.setArg(j++, initialBuffer.clBuffer());  // SCL_INITIAL
  kernel.setArg(j++, tileSizeBytes, NULL);       // SCL_LOCAL
  kernel.setArg(j++, clNumElements);             // SCL_ELEMENTS
  kernel.setArg(j++, clColumnCount);             // SCL_COLS
  detail::kernelUtil::setKernelArgs(kernel, *devicePtr, j,
                                    std::forward<Args>(args)...);
}

template <typename Tin, typename Tout>
void Stencil<Tout(Tin)>::prepareInput(const Matrix<Tin>& input) const
{
  detail::StencilDistribution<Matrix<Tin>> dist(
      _shape.getNorth(), _shape.getWest(), _shape.getSouth(),
      _shape.getEast(), 1);

  input.setDistribution(dist);
  input.createDeviceBuffers();
  input.startUpload();
}

template <typename Tin, typename Tout>
void Stencil<Tout(Tin)>::prepareOutput(Matrix<Tout>& output,
                                       const Matrix<Tin>& input) const
{
  if (output.rowCount() != input.rowCount()) {
    output.resize(typename Matrix<Tout>::size_type(input.rowCount(),
                                                   input.columnCount()));
  }

  output.setDistribution(input.distribution());
  output.createDeviceBuffers();
}

template <typename Tin, typename Tout>
detail::Program Stencil<Tout(Tin)>::createAndBuildProgram() const
{
  ASSERT_MESSAGE(!_userSource.empty(),
                 "Tried to create program with empty user source.");

  std::stringstream temp;

  temp << "#define SCL_NORTH (" << _shape.getNorth() << ")\n"
       << "#define SCL_WEST  (" << _shape.getWest()  << ")\n"
       << "#define SCL_SOUTH (" << _shape.getSouth() << ")\n"
       << "#define SCL_EAST  (" << _shape.getEast()  << ")\n"
       << "#define SCL_TILE_WIDTH  (get_local_size(0) + SCL_WEST + SCL_EAST)\n"
       << "#define SCL_TILE_HEIGHT (get_local_size(1) + SCL_NORTH + SCL_SOUTH)\n"
       << "#define SCL_COL   (get_global_id(0))\n"
       << "#define SCL_ROW   (get_global_id(1))\n"
       << "#define SCL_L_COL (get_local_id(0))\n"
       << "#define SCL_L_ROW (get_local_id(1))\n"
       << "#define SCL_L_COL_COUNT (get_local_size(0))\n"
       << "#define SCL_L_ROW_COUNT (get_local_size(1))\n"
       << "#define SCL_L_ID (SCL_L_ROW * SCL_L_COL_COUNT + SCL_L_COL)\n"
       << "#define SCL_ROWS (SCL_ELEMENTS / SCL_COLS)\n";

  if (_padding == Padding::NEUTRAL)
    temp << "#define NEUTRAL " << _paddingElement << "\n";

  // create program
  std::string s(detail::CommonDefinitions::getSource());
  s.append(Matrix<Tout>::deviceFunctions());
  s.append(temp.str());

  // If all of the boundaries are 0, then this is just a map
  // operation.
  auto ismap = _shape.getNorth() + _shape.getSouth() +
               _shape.getEast() + _shape.getWest() == 0;

  // Helper structs and functions.
  if (ismap) {
    s.append(
        R"(

    typedef float SCL_TYPE_0;
    typedef float SCL_TYPE_1;

    typedef struct {
        __global SCL_TYPE_1* data;
    } input_matrix_t;

    //In case, local memory is used
    SCL_TYPE_1 getData(input_matrix_t* matrix, int x, int y){
        return matrix->data[0];
    }

)");
  } else {
    s.append(
        R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;

typedef struct {
    __local SCL_TYPE_1* data;
} input_matrix_t;

//In case, local memory is used
SCL_TYPE_1 getData(input_matrix_t* matrix, int x, int y){
    int offsetNorth = SCL_NORTH * SCL_TILE_WIDTH;
    int currentIndex = SCL_L_ROW * SCL_TILE_WIDTH + SCL_L_COL;
    int shift = x - y * SCL_TILE_WIDTH;

    return matrix->data[currentIndex+offsetNorth+shift+SCL_WEST];
}

)");
  }

  // Add the user program.
  s.append(_userSource);

  // Append the appropiate kernel, based on the padding type, or if
  // it's a map.
  if (ismap) {
    s.append(
        R"(

__kernel void SCL_STENCIL(__global SCL_TYPE_0* SCL_IN,
                          __global SCL_TYPE_1* SCL_OUT,
                          __global SCL_TYPE_1* SCL_TMP,
                          __local SCL_TYPE_1* SCL_LOCAL_TMP,
                          const unsigned int SCL_ELEMENTS,
                          const unsigned int SCL_COLS)
{
  if (get_global_id(1)*SCL_COLS+get_global_id(0) < SCL_ELEMENTS) {
    input_matrix_t Mm;
    Mm.data = SCL_TMP+get_global_id(1)*SCL_COLS+get_global_id(0);
    SCL_OUT[get_global_id(1)*SCL_COLS+get_global_id(0)] = USR_FUNC(&Mm);
  }
}
)");

  } else {
    // Append macros to define the padding type.
    if (_padding == Padding::NEUTRAL) {
      s.append(R"(
#define STENCIL_PADDING_NEUTRAL         1
#define STENCIL_PADDING_NEAREST         0
#define STENCIL_PADDING_NEAREST_INITIAL 0
)");
    } else if (_padding == Padding::NEAREST) {
      s.append(R"(
#define STENCIL_PADDING_NEUTRAL         0
#define STENCIL_PADDING_NEAREST         1
#define STENCIL_PADDING_NEAREST_INITIAL 0
)");
    } else if (_padding == Padding::NEAREST_INITIAL) {
      s.append(R"(
#define STENCIL_PADDING_NEUTRAL         0
#define STENCIL_PADDING_NEAREST         0
#define STENCIL_PADDING_NEAREST_INITIAL 1
)");
    }

    // Include the kernel definition.
    s.append(
#include "StencilKernel.cl"
             );
  }

  // Build the program.
  auto program =
    detail::Program(s, detail::util::hash("//Stencil\n"
                         + Matrix<Tout>::deviceFunctions()
                         + _userSource + _funcName));
  // Set the parameters and function names.
  if (!program.loadBinary()) {
    program.transferParameters(_funcName, 1, "SCL_STENCIL");
    program.transferArguments(_funcName, 1, "USR_FUNC");
    program.renameFunction(_funcName, "USR_FUNC");
    program.adjustTypes<Tin, Tout>();
  }

  program.build();
  return program;
}

template <typename Tin, typename Tout>
const StencilShape& Stencil<Tout(Tin)>::getShape() const
{
  return this->_shape;
}

template <typename Tin, typename Tout>
const Padding& Stencil<Tout(Tin)>::getPadding() const
{
  return this->_padding;
}

template <typename Tin, typename Tout>
const Tin& Stencil<Tout(Tin)>::getPaddingElement() const
{
  return this->_paddingElement;
}

template <typename Tin, typename Tout>
template <typename T>
StencilSequence<Tout, Tin, T>
  Stencil<Tout(Tin)>::operator<<(const Stencil<Tin(T)>& s) const
{
  return StencilSequence<Tout, Tin, T>(*this,
           StencilSequence<Tin, T>(s,
             StencilSequence<T>()));
}

template <typename Tin, typename Tout>
template <typename T>
StencilSequence<T, Tout, Tin>
  Stencil<Tout(Tin)>::operator>>(const Stencil<T(Tout)>& s) const
{
  return s << *this;
}

template <typename Tin, typename Tout>
StencilSequence<Tout, Tin> Stencil<Tout(Tin)>::toSeq() const {
  return StencilSequence<Tout, Tin>(*this, StencilSequence<Tin>());
}

}  // namespace

#endif   // STENCILDEF_H_
