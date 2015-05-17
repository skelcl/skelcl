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
template<typename > class StencilSequence;

template<typename Tin, typename Tout>
class StencilSequence<Tout(Tin)> : public detail::Skeleton {

public:
  StencilSequence<Tout(Tin)>();

  // Add a new stencil to the sequence.
  void add(Stencil<Tout(Tin)> *const stencil);

  // Run sequence for 1 iterations.
  template<typename ... Args>
  Matrix<Tout> operator()(const Matrix<Tin>& input,
                          Args&&... args) const;

  // Run sequence for n iterations.
  template<typename ... Args>
  Matrix<Tout> operator()(const unsigned int iterations,
                          const Matrix<Tin>& input,
                          Args&&... args) const;

  // Run sequence for n iterations (in-place).
  template<typename ... Args>
  Matrix<Tout>& operator()(const unsigned int iterations,
                           Out<Matrix<Tin>> output,
                           const Matrix<Tin>& input,
                           Args&&... args) const;

private:
  template<typename ... Args>
  Matrix<Tout>& operator()(const unsigned int iterations,
                           Out<Matrix<Tin>> output,
                           Out<Matrix<Tin>> temp,
                           const Matrix<Tin>& input,
                           Args&&... args) const;

  template<typename ... Args>
  void execute(const Matrix<Tin>& input,
               Matrix<Tout>& temp,
               Matrix<Tout>& output,
               const unsigned int iterations,
               Args&&... args) const;

  unsigned int getIterationsUntilNextSync(
      const unsigned int iterationsRemaining) const;

  StencilDirection::type getSumNorthBorders() const;
  StencilDirection::type getSumSouthBorders() const;

  std::vector<Stencil<Tout(Tin)> *> _sequence;
};

} //namespace skelcl

#include "detail/StencilSequenceDef.h"

#endif  // STENCIL_SEQUENCE_H_
