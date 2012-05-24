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
 * SourceRange.cpp
 */

#include <clang-c/Index.h>

#include "ssedit/SourceRange.h"

#include "ssedit/SourceLocation.h"

namespace ssedit {

SourceRange::SourceRange()
  : _range(clang_getNullRange())
{
}

SourceRange::SourceRange(const SourceRange& rhs)
  : _range(rhs._range)
{
}

SourceRange::SourceRange(const SourceLocation& start, const SourceLocation& end)
  : _range(clang_getRange(start.getCXSourceLocation(), end.getCXSourceLocation()))
{
}

SourceRange::SourceRange(CXSourceRange range)
  : _range(range)
{
}

SourceRange& SourceRange::operator=(const SourceRange& rhs)
{
  _range = rhs._range;
  return *this;
}

SourceLocation SourceRange::getStartLocation() const
{
  return SourceLocation(clang_getRangeStart(_range));
}

SourceLocation::Location SourceRange::getStart() const
{
  return SourceLocation(clang_getRangeStart(_range)).getSpellingLocation();
}

SourceLocation SourceRange::getEndLocation() const
{
  return SourceLocation(clang_getRangeEnd(_range));
}

SourceLocation::Location SourceRange::getEnd() const
{
  return SourceLocation(clang_getRangeEnd(_range)).getSpellingLocation();
}

unsigned int SourceRange::length() const
{
  return getEnd().offset - getStart().offset;
}

} // namespace ssedit

