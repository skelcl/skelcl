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
/// \file Stencil.h
///
/// A Stencil operation.
///
/// \author Stefan Breuer<s_breu03@uni-muenster.de>
/// \author Chris Cummins <chrisc.101@gmail.com>
#ifndef STENCIL_H_
#define STENCIL_H_

#include <limits.h>

#include "./detail/Skeleton.h"
#include "./detail/StencilShape.h"

namespace skelcl {

template<typename > class Stencil;
template<typename > class StencilSequence;

template<typename Tin, typename Tout>
class Stencil<Tout(Tin)> : public detail::Skeleton {
public:
  friend class StencilSequence<Tout(Tin)>;

  // Construct a new stencil.
  Stencil(const Source& source,
          const std::string& func,
          const detail::StencilShape& shape,
          detail::Padding padding, Tin paddingElement = NULL);

  // Execute stencil, returning output.
  template <typename... Args>
  Matrix<Tout> operator()(const Matrix<Tin>& input,
                          Args&&... args) const;

  // Execute stencil (in-place).
  template <typename... Args>
  Matrix<Tout>& operator()(Out<Matrix<Tout>> output,
                           const Matrix<Tin>& input,
                           Args&&... args) const;

  // Returns the stencil shape used.
  const detail::StencilShape& getShape() const;

  // Returns the padding method used.
  const detail::Padding& getPadding() const;

  /// Returns neutral element, which shall be used to fill the overlap
  /// region (only used if detail::Padding == NEUTRAL)
  const Tin& getPaddingElement() const;

private:
  template <typename... Args>
  void execute(const Matrix<Tin>& input, Matrix<Tout>& output,
               const Matrix<Tin>& initial,
               const unsigned int opsUntilNextSync,
               const unsigned int opsSinceLastSync,
               const unsigned int sumSouthBorders,
               const unsigned int sumNorthBorders,
               Args&&... args) const;

  // Utility methods.
  detail::Program createAndBuildProgram() const;
  void prepareInput(const Matrix<Tin>& input) const;
  void prepareOutput(Matrix<Tout>& output, const Matrix<Tin>& input) const;
  void getLocalSize(cl_uint *local) const;
  void getTileSize(const cl_uint *local, unsigned int *tile) const;
  void getGlobalSize(const cl_uint *local,
                     const Matrix<Tout>& output,
                     const size_t deviceId,
                     const size_t numDevices,
                     const unsigned int opsUntilNextSync,
                     const unsigned int opsSinceLastSync,
                     const unsigned int sumSouthBorders,
                     const unsigned int sumNorthBorders,
                     cl_uint *global) const;
  template <typename... Args>
  void setKernelArgs(Matrix<Tout>& output,
                     const Matrix<Tin>& input,
                     const Matrix<Tin>& initial,
                     const cl_uint *const localSize,
                     const std::shared_ptr<detail::Device>& devicePtr,
                     cl::Kernel& kernel,
                     Args&&... args) const;

  // Member variables.
  const detail::StencilShape& _shape;
  detail::Padding _padding;
  Tin _paddingElement;

  std::string _userSource;  // Source code from application developer.
  std::string _funcName;    // Name of main function defined in _userSource.
  detail::Program _program;
};

} // skelcl

#include "detail/StencilDef.h"

#endif  // STENCIL_H_
