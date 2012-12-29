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
/// \file MapHelperDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef MAP_HELPER_DEF_H_
#define MAP_HELPER_DEF_H_

#include "../Distributions.h"

#include "Logger.h"
#include "Util.h"
#include "KernelUtil.h"

namespace skelcl {

namespace detail {

template <typename Tin, typename Tout>
MapHelper<Tout(Tin)>::MapHelper(detail::Program&& program)
  : _program(std::move(program))
{
}

template <typename Tin, typename Tout>
template <template <typename> class C>
void MapHelper<Tout(Tin)>::prepareInput(const C<Tin>& input) const
{
  // set default distribution if required
  if (!input.distribution().isValid()) {
    input.setDistribution(BlockDistribution<C<Tin>>());
  }
  // create buffers if required
  input.createDeviceBuffers();
  // copy data to devices
  input.startUpload();
}

template <typename Tin, typename Tout>
template <template <typename> class C>
void MapHelper<Tout(Tin)>::prepareOutput(C<Tout>& output,
                                         const C<Tin>& input) const
{
  if (static_cast<void*>(&output) == static_cast<const void*>(&input)) {
    return; // already prepared in prepareInput
  }
  // resize container if required
  if (output.size() < input.size()) {
    output.resize(input.size());
  }
  // adopt distribution from input
  output.setDistribution(input.distribution());
  // create buffers if required
  output.createDeviceBuffers();
}

} // namespace detail

} // namespace skelcl

#endif // MAP_HELPER_DEF_H_
