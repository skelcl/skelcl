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
/// \file StencilPipelineDef.h
///
///	\author Stefan Breuer<s_breu03@uni-muenster.de>
///	\author Chris Cummins <chrisc.101@gmail.com>
///
#ifndef STENCILPIPELINEDEF_H_
#define STENCILPIPELINEDEF_H_

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

#include "../Distributions.h"
#include "../Matrix.h"
#include "../Out.h"

#include "../Stencil.h"

#include "Device.h"
#include "KernelUtil.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

#include "./StencilKnobs.h"

namespace skelcl {

template <typename Tin, typename Tout>
StencilPipeline<Tout(Tin)>::StencilPipeline(const int iterBetweenSwaps)
  : detail::Skeleton("StencilPipeline<Tout(Tin)>"), _iterBetweenSwaps(iterBetweenSwaps)
{
  LOG_DEBUG_INFO("Create new StencilPipeline object (", this, ")");
}

template <typename Tin, typename Tout>
void StencilPipeline<Tout(Tin)>::add(Stencil<Tout(Tin)> stencil)
{
  _pipeline.push_back(stencil);
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout> StencilPipeline<Tout(Tin)>::operator()(
    const unsigned int iterations,
    const Matrix<Tin>& input,
    Args&&... args)
{
  Matrix<Tout> output;
  this->operator()(iterations, input, out(output), std::forward<Args>(args)...);
  return output;
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout>& StencilPipeline<Tout(Tin)>::operator()(
    const unsigned int iterations,
    const Matrix<Tin>& input,
    Out<Matrix<Tout>> output,
    Args&&... args)
{
  Matrix<Tout> temp;
  return this->operator()(iterations, input, out(temp), output,
                          std::forward<Args>(args)...);
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout>& StencilPipeline<Tout(Tin)>::operator()(
    const unsigned int iterations,
    const Matrix<Tin>& input,
    Out<Matrix<Tout>> temp,
    Out<Matrix<Tout>> output,
    Args&&... args)
{
  ASSERT(iterations > 0);
  ASSERT(input.rowCount() > 0);
  ASSERT(input.columnCount() > 0);

  // Set state.
  _iterations = iterations;

  pvsutil::Timer t; // Time how long it takes to prepare input and output data.

  prepareInput(input);
  prepareAdditionalInput(std::forward<Args>(args)...);
  prepareOutput(output.container(), input);
  prepareOutput(temp.container(), input);

  // Profiling information.
  LOG_PROF(_name, "[", this, "] prepare ", t.stop(), " ms");

  if (_iterations % 2 == 0) {
    execute(input, temp.container(), output.container(),
            std::forward<Args>(args)...);
  } else {
    if ((_iterations % 2 == 1) && (_pipeline.size() % 2 == 0)) {
      execute(input, temp.container(), output.container(),
              std::forward<Args>(args)...);
    } else {
      execute(input, output.container(), temp.container(),
              std::forward<Args>(args)...);
    }
  }

  updateModifiedStatus(temp, output, std::forward<Args>(args)...);

  return output.container();
}

template <typename Tin, typename Tout>
template <typename... Args>
void StencilPipeline<Tout(Tin)>::execute(
    const Matrix<Tin>& input,
    Matrix<Tout>& output,
    Matrix<Tout>& temp,
    Args&&... args)
{
  ASSERT(input.distribution().isValid());
  ASSERT(output.rowCount() == input.rowCount() &&
         output.columnCount() == input.columnCount());

  unsigned int iterationsBetweenSwaps =
      determineIterationsBetweenDataSwaps(input, _iterations);
  unsigned int iterationsAfterLastSync = 0;

  unsigned int numDevices = input.distribution().devices().size();

  unsigned int southSum = determineSouthSum();
  unsigned int northSum = determineNorthSum();

  // Backwards (!) iteration counter. Used to determine the first
  // iteration.
  int k = 1;

  // Helpers to determine whether the current iteration is the first,
  // or an odd/even one.
#define firstIteration(i, k) ((i) + (k) == 0)
#define evenIteration(i, k)  (((i) + (k)) % 2 == 0)
#define oddIteration(i, k)   (((i) + (k)) % 2 == 1)

  try
  {
    for (int i = 0; i < _iterations; i++) {
      k--;

      // Synchronise data across devices.
      if (numDevices != 1 &&
          iterationsAfterLastSync == iterationsBetweenSwaps) {
        // Perform swap.
        if (evenIteration(i, k)) {
          (dynamic_cast<detail::StencilDistribution<Matrix<Tout>>*>(
               &temp.distribution()))->swap(temp, iterationsBetweenSwaps);
        } else if (oddIteration(i, k)) {
          (dynamic_cast<detail::StencilDistribution<Matrix<Tout>>*>(
               &output.distribution()))->swap(output, iterationsBetweenSwaps);
        }

        // Set number of iterations before next swap.
        iterationsBetweenSwaps =
            determineIterationsBetweenDataSwaps(input, _iterations - i - 1);
        iterationsAfterLastSync = 0; // Reset swap counter.
      }

      // Iterate over the list of stencil operations.
      for (auto& stencil : _pipeline) {

        // Execute stencil.
        if (firstIteration(i, k))
          stencil->operator()(input, output, input, northSum, southSum, iterationsBetweenSwaps, iterationsAfterLastSync);
        else if (evenIteration(i, k))
          stencil->operator()(temp, output, input, northSum, southSum, iterationsBetweenSwaps, iterationsAfterLastSync);
        else
          stencil->operator()(output, temp, input, northSum, southSum, iterationsBetweenSwaps, iterationsAfterLastSync);

        k++;
      }
      iterationsAfterLastSync++;
    }
  }
  catch (cl::Error& err)
  {
    ABORT_WITH_ERROR(err);
  }
}

#undef firstIteration
#undef evenIteration
#undef oddIteration

template <typename Tin, typename Tout>
void StencilPipeline<Tout(Tin)>::prepareInput(const Matrix<Tin>& in)
{
  // set distribution
  detail::StencilDistribution<Matrix<Tin>> dist(
      determineNorthSum(), determineWestSum(), determineSouthSum(),
      determineEastSum(), determineIterationsBetweenDataSwaps(in, _iterations));
  in.setDistribution(dist);

  // create buffers if required
	in.createDeviceBuffers();

	// copy data to devices
	in.startUpload();
}

template <typename Tin, typename Tout>
unsigned int StencilPipeline<Tout(Tin)>::determineIterationsBetweenDataSwaps(
    const Matrix<Tin>& /*in*/, unsigned int iterLeft)
{
  // User choses a value
  if (static_cast<int>(iterLeft) <= _iterBetweenSwaps)
    return iterLeft;
  else if (_iterBetweenSwaps != -1)
    return _iterBetweenSwaps;

  // Add the online determination of the number of iterations between device
  // synchronizations here

  return 1;
}

template <typename Tin, typename Tout>
unsigned int StencilPipeline<Tout(Tin)>::determineNorthSum()
{
  unsigned int largestNorth = 0;
  for (auto& stencil : _pipeline)
    largestNorth += stencil->getNorth();

  return largestNorth;
}

template <typename Tin, typename Tout>
unsigned int StencilPipeline<Tout(Tin)>::determineEastSum()
{
  unsigned int largestEast = 0;
  for (auto& stencil : _pipeline)
    largestEast += stencil->getEast();

  return largestEast;
}

template <typename Tin, typename Tout>
unsigned int StencilPipeline<Tout(Tin)>::determineSouthSum()
{
  unsigned int largestSouth = 0;
  for (auto& stencil : _pipeline)
    largestSouth += stencil->getSouth();

  return largestSouth;
}

template <typename Tin, typename Tout>
unsigned int StencilPipeline<Tout(Tin)>::determineWestSum()
{
  unsigned int largestWest = 0;
  for (auto& stencil : _pipeline)
    largestWest += stencil->getWest();

  return largestWest;
}

template <typename Tin, typename Tout>
void StencilPipeline<Tout(Tin)>::prepareOutput(Matrix<Tout>& output,
                                               const Matrix<Tin>& in)
{
  // set size
  if (output.rowCount() != in.rowCount())
    output.resize(
        typename Matrix<Tout>::size_type(in.rowCount(), in.columnCount()));

  // adopt distribution from in input
  output.setDistribution(in.distribution()); // richtiger typ (Tout)?

  // create buffers if required
  output.createDeviceBuffers();
}

} // namespace skelcl

#endif  // STENCILPIPELINEDEF_H_
