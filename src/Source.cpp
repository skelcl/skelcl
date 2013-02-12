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
/// \file Source.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <iostream>

#include <istream>
#include <string>

#include <pvsutil/Assert.h>

#include "SkelCL/Source.h"

namespace skelcl {

Source::Source()
  : _source()
{
}

Source::Source(const char* source)
  : _source(source)
{
  ASSERT_MESSAGE(!_source.empty(),
    "Tried to create source object with empty user source.");
}

Source::Source(const std::string& source)
  : _source(source)
{
  ASSERT_MESSAGE(!_source.empty(),
    "Tried to create source object with empty user source.");
}

Source::Source(std::istream& is)
  : _source( (std::istreambuf_iterator<char>(is)),
              std::istreambuf_iterator<char>()     )
{
  ASSERT_MESSAGE(!_source.empty(),
    "Tried to create source object with empty user source.");
}

Source::Source(std::istream&& is)
  : _source( (std::istreambuf_iterator<char>(is)),
              std::istreambuf_iterator<char>()     )
{
  ASSERT_MESSAGE(!_source.empty(),
    "Tried to create source object with empty user source.");
}

Source::~Source()
{
}

Source::operator std::string() const
{
  return _source;
}

void Source::append(const std::string& source)
{
  _source.append("\n" + source);
}

namespace detail {

CommonDefinitions::CommonDefinitions()
  : _sources(Level::SIZE)
{
}

CommonDefinitions& CommonDefinitions::instance()
{
  static CommonDefinitions instance;
  return instance;
}

void CommonDefinitions::append(const std::string& source, Level level)
{
  CommonDefinitions::instance()._sources[level].append(source);
}

Source CommonDefinitions::getSource()
{
  auto& instance = CommonDefinitions::instance();
  auto s = instance._sources[0];
  for (unsigned int i = 1; i < Level::SIZE; ++i) {
    s.append(instance._sources[i]);
  }
  return s;
}

RegisterCommonDefinition::RegisterCommonDefinition(const char* definition,
                                                   CommonDefinitions::Level l)
{
  // if definition contains the string "double" enable double
  if (std::string(definition).find("double") != std::string::npos) {
    CommonDefinitions::append("#pragma OPENCL EXTENSION cl_khr_fp64 : enable",
                              CommonDefinitions::PRAGMA);
  }
  CommonDefinitions::append(definition, l);
}

} // namespace detail

} // namespace skelcl

