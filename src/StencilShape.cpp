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
/// StencilShape.h
///
/// A Stencil operation.
///
/// \author Chris Cummins <chrisc.101@gmail.com>
#include "SkelCL/detail/StencilShape.h"

#include <utility>

namespace skelcl {

namespace detail {

StencilDirection::StencilDirection(StencilDirection::type val) : _val(val) {}

StencilDirection::type StencilDirection::val() const {
  return _val;
}

StencilShape::StencilShape() : _north(0), _south(0), _east(0), _west(0) {}

StencilDirection::type StencilShape::getNorth() const {
    return _north;
}

StencilDirection::type StencilShape::getSouth() const {
    return _south;
}

StencilDirection::type StencilShape::getEast() const {
    return _east;
}

StencilDirection::type StencilShape::getWest() const {
    return _west;
}

Any::Any(StencilDirection::type val) : StencilDirection(val) {}
North::North(StencilDirection::type val) : StencilDirection(val) {}
South::South(StencilDirection::type val) : StencilDirection(val) {}
East::East(StencilDirection::type val) : StencilDirection(val) {}
West::West(StencilDirection::type val) : StencilDirection(val) {}

Any any(size_t size) {
    return Any(size);
}

North north(size_t size) {
    return North(size);
}

South south(size_t size) {
    return South(size);
}

East east(size_t size) {
    return East(size);
}

West west(size_t size) {
    return West(size);
}

} // namespace detail

} // namespace skelcl
