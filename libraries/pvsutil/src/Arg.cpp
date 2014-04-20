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
/// \file Arg.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <string>
#include <sstream>

#include "pvsutil/cmdline/Arg.h"

#include "pvsutil/detail/pvsutilDll.h"

namespace pvsutil {

namespace cmdline {
  
namespace helper {

// initialization of hasArgument<T>::value as true in .h file

PVSUTIL_API const bool hasArgument<bool>::value = false;

} // namespace helper

BaseArg::BaseArg(const Flags& flags,
                 const Description& des,
                 bool mandatory,
                 bool set)
  : _flags(flags), _description(des), _mandatory(mandatory), _set(set)
{
}

const std::vector<Short>& BaseArg::shortFlags() const
{
  return _flags.getShortFlags();
}

const std::vector<Long>& BaseArg::longFlags() const
{
  return _flags.getLongFlags();
}

std::string BaseArg::flagNames() const
{
  std::stringstream ss;
  for (auto& sF : _flags.getShortFlags()) {
    ss << "-" << sF.getName() << " ";
  }
  for (auto& lF : _flags.getLongFlags()) {
    ss << "--" << lF.getName() << " ";
  }
  auto str = ss.str();
  return str.substr(0, str.size()-1); // remove last space
}

const std::string& BaseArg::description() const
{
  return _description.getDescription();
}

bool BaseArg::isMandatory() const
{
  return _mandatory;
}

bool BaseArg::isSet() const
{
  return _set;
}

} // namespace cmdline

} // namespace pvsutil

