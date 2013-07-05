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
/// \file Flags.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <string>

#include "pvsutil/Assert.h"

#include "pvsutil/cmdline/Flags.h"

namespace pvsutil {

namespace cmdline {

Short::Short(const char c)
  : _name(1, c)
{
}

bool Short::operator<(const Short& rhs) const
{
  return _name < rhs._name;
}

const std::string& Short::getName() const
{
  return _name;
}

Long::Long(const char* name)
  : _name(name)
{
  // ensure no strings are in the flag names
  ASSERT(_name.find(' ') == std::string::npos);
}

Long::Long(const std::string& name)
  : _name(name)
{
}

bool Long::operator<(const Long& rhs) const
{
  return _name < rhs._name;
}

const std::string& Long::getName() const
{
  return _name;
}

void Flags::init()
{
}

const std::vector<Short>& Flags::getShortFlags() const
{
  return _shortFlags;
}

const std::vector<Long>& Flags::getLongFlags() const
{
  return _longFlags;
}

} // namespace cmdline

} // namespace pvsutil

