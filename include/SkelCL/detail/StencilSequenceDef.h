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
///     \author Michel Steuwer <michel.steuwer@ed.ac.uk>
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

namespace detail {
  template <typename...> struct LastT;

  template <typename T>
  struct LastT<T> { typedef T type; };

  template <typename T0, typename T1, typename ...Ts>
  struct LastT<T0, T1, Ts...> { typedef typename LastT<T1, Ts...>::type type; };

  template <typename T0, typename T1>
  StencilSequence<T0, T1> append(const StencilSequence<T0>& /*seq*/,
                                 const Stencil<T0(T1)>& s)
  {
    return StencilSequence<T0, T1>(s, StencilSequence<T1>());
  }

  template <typename T, typename... Ts>
  StencilSequence<Ts..., T>
    append(const StencilSequence<Ts...>& seq,
           const Stencil<typename LastT<Ts...>::type(T)>& s)
  {
    return StencilSequence<Ts..., T>(seq.head(), append(seq.tail(), s));
  }
} // namespace detail

template<typename T0, typename T1, typename... Ts>
StencilSequence<T0, T1, Ts...>::StencilSequence(const Stencil<T0(T1)>& s,
                                                StencilSequence<T1, Ts...> seq)
  : _head(s), _tail(seq)
{
  LOG_DEBUG_INFO("Create new StencilSequence object (", this, ")");
}

template<typename T0, typename T1, typename... Ts>
template<typename T>
StencilSequence<T, T0, T1, Ts...>
  StencilSequence<T0, T1, Ts...>::operator>>(const Stencil<T(T0)>& s)
{
  return StencilSequence<T, T0, T1, Ts...>(s, *this);
}

template<typename T0, typename T1, typename... Ts>
template<typename T>
StencilSequence<T0, T1, Ts..., T>
  StencilSequence<T0, T1, Ts...>::operator<<(const Stencil<TLast(T)>& s)
{
  return detail::append(*this, s);
}

template<typename T0, typename T1, typename... Ts>
const Stencil<T0(T1)>& StencilSequence<T0, T1, Ts...>::head() const
{
  return _head;
}

template<typename T0, typename T1, typename... Ts>
const StencilSequence<T1, Ts...>& StencilSequence<T0, T1, Ts...>::tail() const
{
  return _tail;
}


// execution without iteration

template<typename T0, typename T1, typename... Ts>
template<typename... Args>
Matrix<T0>
StencilSequence<T0, T1, Ts...>::operator()(const Matrix<TLast>& input,
                                           Args&& ... args) const
{
  Matrix<T0> output;
  execute(out(output), input, std::forward<Args>(args)...);
  return output;
}

template<typename T0, typename T1, typename... Ts>
template<typename... Args>
Matrix<T0>&
StencilSequence<T0, T1, Ts...>::operator()(Out<Matrix<T0>> output,
                                           const Matrix<TLast>& input,
                                           Args&& ... args) const
{
  execute(output, input, std::forward<Args>(args)...);
  return output.container();
}

template<typename T0, typename T1, typename... Ts>
template<typename ... Args>
void StencilSequence<T0, T1, Ts...>::execute(Out<Matrix<T0>> output,
                                             const Matrix<TLast>& input,
                                             Args&& ... args) const
{
  Matrix<T1> tmp;
  // first execute tail ...
  auto& tailOutput = _tail(out(tmp), input, std::forward<Args>(args)...);
  // ... then execute the stencil head
  _head(output, tailOutput, std::forward<Args>(args)...);
}


// execution with iteration

template<typename T0, typename T1, typename... Ts>
template<typename... Args>
Matrix<T0>
  StencilSequence<T0, T1, Ts...>::operator()(int iterations,
                                             const Matrix<TLast>& input,
                                             Args&& ... args) const
{
  static_assert(std::is_same<T0, TLast>::value,
                "First and last type have to be the same for performing "
                        "iterations.");
  Matrix<T0> output;
  prepareAndExecuteIter(iterations, out(output), input,
                        std::forward<Args>(args)...);
  return output;
}

template<typename T0, typename T1, typename... Ts>
template<typename... Args>
Matrix<T0>&
  StencilSequence<T0, T1, Ts...>::operator()(int iterations,
                                             Out<Matrix<T0>> output,
                                             const Matrix<TLast>& input,
                                             Args&& ... args) const
{
  static_assert(std::is_same<T0, TLast>::value,
                "First and last type have to be the same for performing "
                        "iterations.");
  prepareAndExecuteIter(iterations, output, input, std::forward<Args>(args)...);
  return output.container();
}

template<typename T0, typename T1, typename... Ts>
template<typename ... Args>
void StencilSequence<T0, T1, Ts...>::
  prepareAndExecuteIter(int iterations,
                        Out<Matrix<T0>> output,
                        const Matrix<TLast>& input,
                        Args&& ... args) const
{
  auto sumNorthBorders = getSumNorthBorders();
  auto sumSouthBorders = getSumSouthBorders();

  int iterationsSinceLastSync = 0;
  int iterationsUntilNextSync = getIterationsUntilNextSync(iterations);

  Matrix<T0> secondBuffer; // swap buffer

  Matrix<T0>* buffer0Ptr;
  Matrix<T0>* buffer1Ptr;

  if (iterations % 2 != 0) { // odd number of iterations
    buffer0Ptr = &output.container();
    buffer1Ptr = &secondBuffer;
  } else { // even number of iterations
    buffer0Ptr = &secondBuffer;
    buffer1Ptr = &output.container();
  }

  executeIter(iterations, 0, sumNorthBorders, sumSouthBorders,
              iterationsSinceLastSync, iterationsUntilNextSync,
              *buffer0Ptr, *buffer1Ptr, input,
              std::forward<Args>(args)...);
}

template<typename T0, typename T1, typename... Ts>
template <typename... Args>
void StencilSequence<T0, T1, Ts...>::
  executeIter(int iterationsLeft,
              int iteration,
              StencilDirection::type sumNorthBorders,
              StencilDirection::type sumSouthBorders,
              int iterationsSinceLastSync,
              int iterationsUntilNextSync,
              Matrix<T0>& buffer0,
              Matrix<T0>& buffer1,
              const Matrix<TLast>& input,
              Args&&... args) const
{
  LOG_INFO("Perform iteration: ", iteration);
  auto numDevices = input.distribution().devices().size();

  // Perform synchronisation across multiple devices
  if (numDevices > 1 && iterationsUntilNextSync) {
    if (iteration % 2 != 0) {
      // even iteration: Swap buffer0 (the input)
      auto ptr = (dynamic_cast<detail::StencilDistribution<Matrix<T0>>*>(
              &buffer0.distribution()));
      ptr->swap(buffer0, iterationsSinceLastSync);
    } else {
      // odd iteration: Swap buffer1 (the input)
      auto ptr = (dynamic_cast<detail::StencilDistribution<Matrix<T0>>*>(
              &buffer1.distribution()));
      ptr->swap(buffer1, iterationsSinceLastSync);
    }

    // Reset counters
    iterationsUntilNextSync = getIterationsUntilNextSync(iterationsLeft - 1);
    iterationsSinceLastSync = 0;
  }

  if (iteration == 0) {
    // first iteration: set input as input and buffer0 as output
    execute(buffer0, input, input,
            sumNorthBorders, sumSouthBorders,
            iterationsUntilNextSync, iterationsSinceLastSync,
            std::forward<Args>(args)...);
  } else if (iteration % 2 != 0) {
    // odd iterations: set buffer0 as input and buffer1 as output
    execute(buffer1, buffer0, input,
            sumNorthBorders, sumSouthBorders,
            iterationsUntilNextSync, iterationsSinceLastSync,
            std::forward<Args>(args)...);
  } else {
    // even iterations: set buffer1 as input and buffer0 as output
    execute(buffer0, buffer1, input,
            sumNorthBorders, sumSouthBorders,
            iterationsUntilNextSync, iterationsSinceLastSync,
            std::forward<Args>(args)...);
  }

  // last iteration just completed ...
  if (iterationsLeft == 1) {
    // ... end iteration
    return;
  }

  // ... otherwise, perform next iteration
  executeIter(iterationsLeft - 1, iteration + 1,
              sumNorthBorders, sumSouthBorders,
              iterationsSinceLastSync + 1, iterationsUntilNextSync - 1,
              buffer0, buffer1, input, std::forward<Args>(args)...);
}

template<typename T0, typename T1, typename... Ts>
template <typename... Args>
const Matrix<T0>& StencilSequence<T0, T1, Ts...>::
  execute(Matrix<T0>& output,
          const Matrix<TLast>& input,
          const Matrix<TLast>& initial,
          StencilDirection::type sumNorthBorders,
          StencilDirection::type sumSouthBorders,
          int iterationsSinceLastSync,
          int iterationsUntilNextSync,
          Args&&... args) const
{
  Matrix<T1> tmp;
  // first execute tail ...
  auto& tailOutput = _tail.execute(tmp, input, initial,
                                   sumNorthBorders, sumSouthBorders,
                                   iterationsSinceLastSync,
                                   iterationsUntilNextSync,
                                   std::forward<Args>(args)...);
  // ... then execute the stencil head
  _head.execute(tailOutput, output, initial,
                iterationsUntilNextSync, iterationsSinceLastSync,
                sumNorthBorders, sumSouthBorders,
                std::forward<Args>(args)...);
  return output;
}

template <typename T0, typename T1, typename... Ts>
StencilDirection::type
  StencilSequence<T0, T1, Ts...>::getSumSouthBorders() const
{
  return _head.getShape().getSouth() + _tail.getSumSouthBorders();
}

template <typename T0, typename T1, typename... Ts>
StencilDirection::type
  StencilSequence<T0, T1, Ts...>::getSumNorthBorders() const
{
  return _head.getShape().getNorth() + _tail.getSumNorthBorders();
}

template <typename T0, typename T1, typename... Ts>
int StencilSequence<T0, T1, Ts...>::
  getIterationsUntilNextSync(int iterationsRemaining) const
{
  return std::min(STENCIL_ITERATIONS_BETWEEN_SWAPS, iterationsRemaining);
}


// base case
template<typename T>
template<typename... Args>
const Matrix<T>& StencilSequence<T>::operator()(Out<Matrix<T>> /*output*/,
                                                const Matrix<T>& input,
                                                Args&& ... /*args*/) const
{
  return input;
}

template<typename T>
template <typename... Args>
const Matrix<T>& StencilSequence<T>::execute(Matrix<T>&,
                                             const Matrix<T>& input,
                                             const Matrix<T>&,
                                             StencilDirection::type,
                                             StencilDirection::type,
                                             int, int, Args&&...) const
{
  return input;
}

template<typename T>
StencilDirection::type StencilSequence<T>::getSumSouthBorders() const
{
  return 0;
}

template<typename T>
StencilDirection::type StencilSequence<T>::getSumNorthBorders() const
{
  return 0;
}

}  // namespace skelcl

#endif  // STENCIL_SEQUENCEDEF_H_
