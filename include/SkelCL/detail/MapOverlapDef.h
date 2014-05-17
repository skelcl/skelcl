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
/// \file MapOverlapDef.h
///
///	\author Stefan Breuer<s_breu03@uni-muenster.de>
///
#ifndef MAPOVERLAPDEF_H_
#define MAPOVERLAPDEF_H_

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

#include "../Distributions.h"
#include "../Matrix.h"
#include "../Reduce.h"
#include "../Zip.h"
#include "../Out.h"

#include "Device.h"
#include "KernelUtil.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

namespace skelcl {

template <typename Tin, typename Tout>
MapOverlap<Tout(Tin)>::MapOverlap(const Source& source,
                                  unsigned int overlap_range,
                                  detail::Padding padding, Tin neutral_element,
                                  const std::string& func)
  : detail::Skeleton(), _userSource(source), _funcName(func),
    _overlap_range(overlap_range), _padding(padding),
    _neutral_element(neutral_element), _program(createAndBuildProgram())
{
  LOG_DEBUG_INFO("Create new MapOverlap object (", this, ")");
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout> MapOverlap<Tout(Tin)>::operator()(const Matrix<Tin>& in,
                                               Args&&... args)
{
  Matrix<Tout> output;
  this->operator()(out(output), in, std::forward<Args>(args)...);
	return output;
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout>& MapOverlap<Tout(Tin)>::
    operator()(Out<Matrix<Tout>> output, const Matrix<Tin>& in, Args&&... args)
{
  ASSERT(in.rowCount() > 0);
  ASSERT(in.columnCount() > 0);

  prepareInput(in);

  prepareAdditionalInput(std::forward<Args>(args)...);

	prepareOutput(output.container(), in);

	execute(output.container(), in, std::forward<Args>(args)...);

  updateModifiedStatus(output, std::forward<Args>(args)...);

  return output.container();
}

template <typename Tin, typename Tout>
template <typename... Args>
void MapOverlap<Tout(Tin)>::execute(Matrix<Tout>& output, const Matrix<Tin>& in,
                                    Args&&... args)
{
  ASSERT(in.distribution().isValid());
  ASSERT(output.rowCount() == in.rowCount() &&
         output.columnCount() == in.columnCount());

  for (auto& devicePtr : in.distribution().devices()) {
    cl::Kernel kernel(_program.kernel(*devicePtr, "SCL_MAPOVERLAP"));

    cl_uint workgroupSize = static_cast<cl_uint>(
        detail::kernelUtil::determineWorkgroupSizeForKernel(kernel,
                                                            *devicePtr));

    auto& outputBuffer = output.deviceBuffer(*devicePtr);
    auto& inputBuffer = in.deviceBuffer(*devicePtr);

    cl_uint elements = static_cast<cl_uint>(
        inputBuffer.size() - 2 * _overlap_range * in.columnCount());
    cl_uint local[2] = {static_cast<cl_uint>(sqrt(workgroupSize)), local[0]};
    cl_uint global[2] = {static_cast<cl_uint>(detail::util::ceilToMultipleOf(
                             in.columnCount(), local[0])),
                         static_cast<cl_uint>(detail::util::ceilToMultipleOf(
                             elements / in.rowCount(), local[1]))};

    LOG_DEBUG_INFO("elements: ", elements, " overlap: ", _overlap_range);
    LOG_DEBUG_INFO("local: ", local[0], ",", local[1], " global: ", global[0],
                   ",", global[1]);

    unsigned int tileWidth = local[0] + 2 * _overlap_range;

    try
    {
      int j = 0;
      kernel.setArg(j++, inputBuffer.clBuffer());
      kernel.setArg(j++, outputBuffer.clBuffer());
      kernel.setArg(j++, tileWidth * tileWidth * sizeof(Tin),
                    NULL); // allocate local memory
      kernel.setArg(j++, elements);
      kernel.setArg(j++, static_cast<cl_uint>(output.columnCount()));

      detail::kernelUtil::setKernelArgs(kernel, *devicePtr, j++,
                                        std::forward<Args>(args)...);

      // keep buffers and arguments alive / mark them as in use
      auto keepAlive = detail::kernelUtil::keepAlive(
          *devicePtr, inputBuffer.clBuffer(), outputBuffer.clBuffer(),
          std::forward<Args>(args)...);

      // after finishing the kernel invoke this function ...
      auto invokeAfter = [=]() { (void)keepAlive; };
      auto event = devicePtr->enqueue(kernel, cl::NDRange(global[0], global[1]),
                                      cl::NDRange(local[0], local[1]),
                                      cl::NullRange, // offset
                                      invokeAfter);
    }
    catch (cl::Error& err)
    {
      ABORT_WITH_ERROR(err);
    }
  }
  LOG_INFO("MapOverlap kernel started");
}

template <typename Tin, typename Tout>
detail::Program MapOverlap<Tout(Tin)>::createAndBuildProgram() const
{
  ASSERT_MESSAGE(!_userSource.empty(),
                 "Tried to create program with empty user source.");

  std::stringstream temp;

  temp << "#define SCL_OVERLAP_RANGE (" << _overlap_range << ")\n"
       << "#define SCL_TILE_WIDTH (get_local_size(0) + "
       << "2*" << _overlap_range << ")\n";
  if (_padding == detail::Padding::NEUTRAL) {
    temp << "#define NEUTRAL (" << _neutral_element << ")\n";
  }

  // create program
  std::string s(Matrix<Tout>::deviceFunctions());
  s.append(temp.str());

  // helper structs and functions
  s.append(R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;

typedef struct {
    const __global SCL_TYPE_1* data;
    unsigned int cols;
} output_matrix_t;

typedef struct {
    __local SCL_TYPE_1* data;
    int local_row;
    int local_column;
} input_matrix_t;

SCL_TYPE_1 getData(input_matrix_t* matrix, int x, int y)
{
    //int offsetNorth = SCL_OVERLAP_RANGE * SCL_TILE_WIDTH;
    //int currentIndex = matrix->local_row * SCL_TILE_WIDTH + matrix->local_column;
    //int shift = x - y * SCL_TILE_WIDTH;

    //return matrix->data[currentIndex + offsetNorth + shift + SCL_OVERLAP_RANGE];
    const unsigned int l_col = matrix->local_column;
    const unsigned int l_row = matrix->local_row;
    if (get_global_id(0) == 30) printf("id: 30; index: %d\n", ((y + l_row) + SCL_OVERLAP_RANGE) * SCL_TILE_WIDTH + SCL_OVERLAP_RANGE + (l_col + x));
    if (get_global_id(0) == 30) printf("x: %d; y: %d\n", x, y);
  return matrix->data[((y + l_row) + SCL_OVERLAP_RANGE) * SCL_TILE_WIDTH + SCL_OVERLAP_RANGE + (l_col + x)];
}


)");

  // user source
	s.append(_userSource);

  // allpairs skeleton source
  s.append(
#include "MapOverlapKernel.cl"
      );

  auto program = detail::Program(s, detail::util::hash("//MapOverlap\n" + s));

  // modify program
	if (!program.loadBinary()) {
		program.transferParameters(_funcName, 1, "SCL_MAPOVERLAP");
		program.transferArguments(_funcName, 1, "USR_FUNC");

		program.renameFunction(_funcName, "USR_FUNC");

		program.adjustTypes<Tin, Tout>();
	}
	program.build();

	return program;
}

template <typename Tin, typename Tout>
void MapOverlap<Tout(Tin)>::prepareInput(const Matrix<Tin>& in)
{
  // set distribution
  in.setDistribution(detail::OLDistribution<Matrix<Tin>>(
      _overlap_range, _padding, _neutral_element));

  // create buffers if required
  in.createDeviceBuffers();

  // copy data to devices
  in.startUpload();
}

// Ausgabe vorbereiten
template <typename Tin, typename Tout>
void MapOverlap<Tout(Tin)>::prepareOutput(Matrix<Tout>& output,
                                          const Matrix<Tin>& in)
{
  // set size
  if (output.rowCount() != in.rowCount())
    output.resize(
        typename Matrix<Tout>::size_type(in.rowCount(), in.columnCount()));

  // adopt distribution from in input
  output.setDistribution(in.distribution()); // richtiger typ (Tout)?

  //create buffers if required
  output.createDeviceBuffers();
}
}

#endif /* MAPOVERLAPDEF_H_ */
