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
/// \file StencilSequence.h
///
/// A sequence of stencil skeletons for iterative execution.
///
/// \author Stefan Breuer<s_breu03@uni-muenster.de>
/// \author Chris Cummins <chrisc.101@gmail.com>
/// \author Michel Steuwer <michel.steuwer@ed.ac.uk>
///
#ifndef STENCIL_SEQUENCE_H_
#define STENCIL_SEQUENCE_H_

#include <istream>
#include <string>
#include <vector>

#include "./Padding.h"
#include "./detail/Skeleton.h"
#include "./detail/Program.h"

#include "./Source.h"
#include "./Stencil.h"

namespace skelcl {

template<typename > class Matrix;
template<typename > class Out;
namespace detail { template <typename...> struct LastT; }
template <typename...> class StencilSequence;

template <typename T0, typename T1, typename... Ts>
class StencilSequence<T0, T1, Ts...> {
public:
  typedef typename detail::LastT<T1, Ts...>::type TLast;

  StencilSequence(const Stencil<T0(T1)>& s, StencilSequence<T1, Ts...> seq);

  StencilSequence(const StencilSequence&) = default;

  template <typename... Args>
  Matrix<T0> operator()(const Matrix<TLast>& input,
                        Args&&... args) const;

  template <typename... Args>
  Matrix<T0>& operator()(Out<Matrix<T0>> output,
                         const Matrix<TLast>& input,
                         Args&&... args) const;

  template <typename... Args>
  Matrix<T0> operator()(int iterations,
                        const Matrix<TLast>& input,
                        Args&&... args) const;

  template <typename... Args>
  Matrix<T0>& operator()(int iterations,
                         Out<Matrix<T0>> output,
                         const Matrix<TLast>& input,
                         Args&&... args) const;

  template <typename T>
  StencilSequence<T, T0, T1, Ts...> operator>>(const Stencil<T(T0)>& s);

  template <typename T>
  StencilSequence<T0, T1, Ts..., T> operator<<(const Stencil<TLast(T)>& s);

  const Stencil<T0(T1)>& head() const;

  const StencilSequence<T1, Ts...>& tail() const;

  // fried classes do not work with template specialization ...
  // ..., therefore, the next three functions are public
  StencilDirection::type getSumSouthBorders() const;

  StencilDirection::type getSumNorthBorders() const;

  template <typename... Args>
  const Matrix<T0>& execute(Matrix<T0>& output,
                            const Matrix<TLast>& input,
                            const Matrix<TLast>& initial,
                            StencilDirection::type sumNorthBorders,
                            StencilDirection::type sumSouthBorders,
                            int iterationsSinceLastSync,
                            int iterationsUntilNextSync,
                            Args&&... args) const;

private:
  int getIterationsUntilNextSync(int iterationsRemaining) const;

  template <typename... Args>
  void execute(Out<Matrix<T0>> output,
               const Matrix<TLast>& input,
               Args&&... args) const;

  template <typename... Args>
  void prepareAndExecuteIter(int iterations,
                             Out<Matrix<T0>> output,
                             const Matrix<TLast>& input,
                             Args&&... args) const;

  template <typename... Args>
  void executeIter(int iterationsLeft,
                   int iteration,
                   StencilDirection::type sumNorthBorders,
                   StencilDirection::type sumSouthBorders,
                   int iterationsSinceLastSync,
                   int iterationsUntilNextSync,
                   Matrix<T0>& buffer0,
                   Matrix<T0>& buffer1,
                   const Matrix<TLast>& input,
                   Args&&... args) const;

  Stencil<T0(T1)> _head;
  StencilSequence<T1, Ts...> _tail;
};

// base case
template <typename T>
class StencilSequence<T> {
public:
  template <typename... Args>
  const Matrix<T>& operator()(Out<Matrix<T>> output,
                              const Matrix<T>& input,
                              Args&&... args) const;

  template <typename... Args>
  const Matrix<T>& execute(Matrix<T>& output,
                           const Matrix<T>& input,
                           const Matrix<T>& initial,
                           StencilDirection::type sumNorthBorders,
                           StencilDirection::type sumSouthBorders,
                           int iterationsSinceLastSync,
                           int iterationsUntilNextSync,
                           Args&&... args) const;

  StencilDirection::type getSumSouthBorders() const;

  StencilDirection::type getSumNorthBorders() const;
};

} //namespace skelcl

#include "detail/StencilSequenceDef.h"

#endif  // STENCIL_SEQUENCE_H_
