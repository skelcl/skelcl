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

/*
 * SourceLocation.cpp
 */
#include <clang-c/Index.h>

#include "ssedit/SourceLocation.h"

#include "ssedit/SourceFile.h"

namespace ssedit {

SourceLocation::SourceLocation()
  : _location(clang_getNullLocation())
{
}

SourceLocation::SourceLocation(const SourceLocation& rhs)
  : _location(rhs._location)
{
}

SourceLocation::SourceLocation(CXSourceLocation location)
  : _location(location)
{
}

SourceLocation& SourceLocation::operator=(const SourceLocation& rhs)
{
  _location = rhs._location;
  return *this;
}

SourceLocation::Location SourceLocation::getSpellingLocation() const
{
  CXFile file;
  unsigned int line,col,off;
  clang_getSpellingLocation(_location,&file,&line,&col,&off);
  Location loc;
  loc.line    = line;
  loc.column  = col;
  loc.offset  = off;
  loc.file    = file;
  return loc;
}

SourceLocation::Location SourceLocation::getInstantiationLocation() const
{
  CXFile file;
  unsigned int line,col,off;
  clang_getInstantiationLocation(_location,&file,&line,&col,&off);
  Location loc;
  loc.line    = line;
  loc.column  = col;
  loc.offset  = off;
  loc.file    = file;
  return loc;
}

CXSourceLocation SourceLocation::getCXSourceLocation() const
{
  return _location;
}

} // namespace ssedit

