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
/// \file MapHelper.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef MAP_HELPER_H_
#define MAP_HELPER_H_

#include <string>

#include "Program.h"

namespace skelcl {

namespace detail {

template<typename> class Container;

template<typename> class MapHelper;

template<typename Tin, typename Tout>
class MapHelper<Tout(Tin)> {
public:
  MapHelper<Tout(Tin)>(const Program& program, const std::string& funcName);

  MapHelper<Tout(Tin)>(const MapHelper<Tout(Tin)>&) = default;

  MapHelper<Tout(Tin)>& operator=(const MapHelper<Tout(Tin)>&) = default;

  ~MapHelper<Tout(Tin)>() = default;

protected:
  void prepareAndBuildProgram(const std::string& funcName);

  void prepareInput(const Container<Tin>& input);

  void prepareOutput(Container<Tout>& output,
                     const Container<Tin>& input);

  Program _program;
};

} // namespace detail

} // namespace skelcl

#include "MapHelperDef.h"

#endif // MAP_HELPER_H_
