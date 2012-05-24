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
 * Typedef.cpp
 */
#include <string>
#include <sstream>
#include "ssedit/Cursor.h"
#include "ssedit/SourceLocation.h"

#include "ssedit/Typedef.h"

namespace ssedit {

Typedef::Typedef(const Cursor& cursor, SourceFile& sourceFile)
  : Entity(cursor, sourceFile) {
}

Typedef::Typedef(const Typedef& rhs)
  : Entity(rhs) {
}

Typedef& Typedef::operator=(const Typedef& rhs) {
  Entity::operator=(rhs);
  return *this;
}

Type Typedef::getType() const {
  return _cursor.getType();
}

std::string Typedef::printInfo() {
  std::stringstream sstr;
  SourceLocation::Location start = _cursor.getExtent().getStart();
  SourceLocation::Location end = _cursor.getExtent().getEnd();
  sstr << "start: ";
  sstr << "Line:   "  << start.line   << " "
       << "Column: "  << start.column << " "
       << "Offset: "  << start.offset << " ";
  sstr << "end: ";
  sstr << "Line:   "  <<  end.line   << " "
       << "Column: "  <<  end.column << " "
       << "Offset: "  <<  end.offset;
  return sstr.str();
}

} // namespace ssedit

