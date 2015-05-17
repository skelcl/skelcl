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
/// \file StencilSequenceDef.h
///
///     \author Stefan Breuer<s_breu03@uni-muenster.de>
///     \author Chris Cummins <chrisc.101@gmail.com>
///
#ifndef STENCIL_SEQUENCEDEF_H_
#define STENCIL_SEQUENCEDEF_H_

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
StencilSequence<Tout(Tin)>::StencilSequence()
    : detail::Skeleton("StencilSequence<Tout(Tin)>"), _sequence() {
  LOG_DEBUG_INFO("Create new StencilSequence object (", this, ")");
}

template <typename Tin, typename Tout>
void StencilSequence<Tout(Tin)>::add(Stencil<Tout(Tin)> *const stencil) {
  _sequence.push_back(stencil);
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout> StencilSequence<Tout(Tin)>::operator()(const Matrix<Tin>& input,
                                                    Args&&... args) const {
  return this->operator()(1, input, std::forward<Args>(args)...);
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout> StencilSequence<Tout(Tin)>::operator()(const unsigned int iterations,
                                                    const Matrix<Tin>& input,
                                                    Args&&... args) const {
  Matrix<Tout> output;
  this->operator()(iterations, out(output), input, std::forward<Args>(args)...);
  return output;
}

template <typename Tin, typename Tout>
template <typename... Args>
Matrix<Tout>& StencilSequence<Tout(Tin)>::operator()(const unsigned int iterations,
                                                     Out<Matrix<Tin>> output,
                                                     const Matrix<Tin>& input,
                                                     Args&&... args) const {
  Matrix<Tout> temp;
  return this->operator()(iterations, output, out(temp), input,
                          std::forward<Args>(args)...);
}

template <typename Tin, typename Tout>
template <typename ... Args>
Matrix<Tout>& StencilSequence<Tout(Tin)>::operator()(const unsigned int iterations,
                                                     Out<Matrix<Tin>> output,
                                                     Out<Matrix<Tin>> temp,
                                                     const Matrix<Tin>& input,
                                                     Args&&... args) const {
  execute(input, temp.container(), output.container(), iterations,
          std::forward<Args>(args)...);

  return output.container();
}

namespace {

}

template <typename Tin, typename Tout>
template <typename... Args>
void StencilSequence<Tout(Tin)>::execute(const Matrix<Tin>& input,
                                         Matrix<Tout>& temp,
                                         Matrix<Tout>& output,
                                         const unsigned int iterations,
                                         Args&&... args) const {
  const unsigned int numDevices = input.distribution().devices().size();
  const unsigned int numOps = iterations * _sequence.size();

  const StencilDirection::type sumSouthBorders = getSumSouthBorders();
  const StencilDirection::type sumNorthBorders = getSumNorthBorders();

  int opsCounter = 0;
  int iterationsSinceLastSync = 0;
  int iterationsUntilNextSync = getIterationsUntilNextSync(iterations);

  ASSERT(numOps > 0);

  for (size_t i = 0; i < iterations; i++) {

    // Perform synchronisation across multiple devices.
    if (numDevices > 1 && iterationsUntilNextSync) {
      if (opsCounter % 2) {  // Even iteration.
        (dynamic_cast<detail::StencilDistribution<Matrix<Tout>>*>(
            &temp.distribution()))->swap(temp, iterationsSinceLastSync);
      } else {               // Odd iteration.
        (dynamic_cast<detail::StencilDistribution<Matrix<Tout>>*>(
            &output.distribution()))->swap(output, iterationsSinceLastSync);
      }

      // Reset counters.
      iterationsUntilNextSync = getIterationsUntilNextSync(iterations - i);
      iterationsSinceLastSync = 0;
    }

    for (auto stencil : _sequence) {
      if (opsCounter == 0)      // First iteration.
        stencil->execute(input, output, input, iterationsUntilNextSync,
                         iterationsSinceLastSync, sumSouthBorders,
                         sumNorthBorders, std::forward<Args>(args)...);
      else if (opsCounter % 2)  // Odd iteration.
        stencil->execute(output, temp, input, iterationsUntilNextSync,
                         iterationsSinceLastSync, sumSouthBorders,
                         sumNorthBorders, std::forward<Args>(args)...);
      else                      // Even iteration.
        stencil->execute(temp, output, input, iterationsUntilNextSync,
                         iterationsSinceLastSync, sumSouthBorders,
                         sumNorthBorders, std::forward<Args>(args)...);

      opsCounter++;
    }

    iterationsSinceLastSync++;
    iterationsUntilNextSync--;
  }
}

template <typename Tin, typename Tout>
StencilDirection::type StencilSequence<Tout(Tin)>::getSumSouthBorders() const {
  StencilDirection::type sum = 0;

  for (auto stencil : _sequence)
    sum += stencil->_shape.getSouth();

  return sum;
}

template <typename Tin, typename Tout>
StencilDirection::type StencilSequence<Tout(Tin)>::getSumNorthBorders() const {
  StencilDirection::type sum = 0;

  for (auto stencil : _sequence)
    sum += stencil->_shape.getNorth();

  return sum;
}

template <typename Tin, typename Tout>
unsigned int StencilSequence<Tout(Tin)>::getIterationsUntilNextSync(
    const unsigned int iterationsRemaining) const
{
  return std::min(static_cast<unsigned int>(STENCIL_ITERATIONS_BETWEEN_SWAPS),
                  iterationsRemaining);
}

}  // namespace skelcl

#endif  // STENCIL_SEQUENCEDEF_H_
