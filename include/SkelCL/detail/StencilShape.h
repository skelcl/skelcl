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
#ifndef STENCIL_SHAPE_H_
#define STENCIL_SHAPE_H_

#include <cstring>
#include <utility>

#include <pvsutil/Logger.h>

#include "./skelclDll.h"

namespace skelcl {

namespace detail {

class SKELCL_DLL StencilDirection {
public:
  typedef size_t type;

  explicit StencilDirection(type val);
  virtual ~StencilDirection() {};

  type val() const;

private:
  const type _val;
};

// Stencil directions.

class SKELCL_DLL Any   : public StencilDirection {
public:
  explicit Any(StencilDirection::type val);
};

class SKELCL_DLL North : public StencilDirection {
public:
  explicit North(StencilDirection::type val);
};

class SKELCL_DLL South : public StencilDirection {
public:
  explicit South(StencilDirection::type val);
};

class SKELCL_DLL East  : public StencilDirection {
public:
  explicit East(StencilDirection::type val);
};

class SKELCL_DLL West  : public StencilDirection {
public:
  explicit West(StencilDirection::type val);
};

class SKELCL_DLL StencilShape {
public:
  StencilShape();

  StencilDirection::type getNorth() const;
  StencilDirection::type getSouth() const;
  StencilDirection::type getEast()  const;
  StencilDirection::type getWest()  const;

  template <typename... Args>
  friend void setStencilDirections(StencilShape &shape, Any any, Args... args);

  template <typename... Args>
  friend void setStencilDirections(StencilShape &shape, North north, Args... args);

  template <typename... Args>
  friend void setStencilDirections(StencilShape &shape, South south, Args... args);

  template <typename... Args>
  friend void setStencilDirections(StencilShape &shape, East east, Args... args);

  template <typename... Args>
  friend void setStencilDirections(StencilShape &shape, West west, Args... args);

  template <typename T, typename... Args>
  friend void setStencilDirections(StencilShape &shape, T direction, Args... args);

  template <typename... Args>
  friend void setStencilDirections(StencilShape &shape, Args... args);

private:
  StencilDirection::type _north;
  StencilDirection::type _south;
  StencilDirection::type _east;
  StencilDirection::type _west;
};


// Stencil direction factoriy methods.

SKELCL_DLL Any   any(size_t size);
SKELCL_DLL North north(size_t size);
SKELCL_DLL South south(size_t size);
SKELCL_DLL East  east(size_t size);
SKELCL_DLL West  west(size_t size);


// Generate a stencil shape from one or more StencilDirection objects.
template <typename T, typename... Args>
StencilShape stencilShape(T direction, Args... args);


// Definitions.

template <typename... Args>
void setStencilDirections(StencilShape &shape, Any any, Args... args) {
  shape._north = any.val();
  shape._south = any.val();
  shape._east = any.val();
  shape._west = any.val();
  setStencilDirections(shape, args...);
}

template <typename... Args>
void setStencilDirections(StencilShape &shape, North north, Args... args) {
  shape._north = north.val();
  setStencilDirections(shape, args...);
}

template <typename... Args>
void setStencilDirections(StencilShape &shape, South south, Args... args) {
  shape._south = south.val();
  setStencilDirections(shape, args...);
}

template <typename... Args>
void setStencilDirections(StencilShape &shape, East east, Args... args) {
  shape._east = east.val();
  setStencilDirections(shape, args...);
}

template <typename... Args>
void setStencilDirections(StencilShape &shape, West west, Args... args) {
  shape._west = west.val();
  setStencilDirections(shape, args...);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

template <typename... Args>
void setStencilDirections(StencilShape &shape, Args... args) {
  // Bottom out case, no more directions to set.
}

template <typename T, typename... Args>
void setStencilDirections(StencilShape &shape, T direction, Args... args) {
  LOG_ERROR("Stencil ", shape._north, " ", shape._south, " ", shape._east, " ", shape._west);
  ABORT_WITH_ERROR("Unrecognised argument while setting stencil direction.");
}

#pragma GCC diagnostic pop

template <typename T, typename... Args>
StencilShape stencilShape(T direction, Args... args) {
  StencilShape shape = StencilShape();

  setStencilDirections(shape, direction, args...);

  return shape;
}

} // namespace detail

} // namespace skelcl

#endif // STENCIL_SHAPE_H_
