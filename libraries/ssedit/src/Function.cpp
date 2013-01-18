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
 * Function.cpp
 */

#include <algorithm>
#include <vector>

#include <pvsutil/Logger.h>

#include "ssedit/Function.h"

#include "ssedit/Entity.h"
#include "ssedit/Parameter.h"
#include "ssedit/Type.h"
#include "ssedit/Cursor.h"

namespace { // unnamed namespace

using namespace ssedit;

struct IsParameter {
  bool operator()(const Cursor& cursor)
  {
    return cursor.isOfKind(CXCursor_ParmDecl);
  }
};

struct IsFirstCompoundStmt {
  IsFirstCompoundStmt() : found(false) {}
  bool found;

  bool operator()(const Cursor& cursor)
  {
    if (!found && cursor.isOfKind(CXCursor_CompoundStmt)) {
      found = true;
      return true;
    }
    return false;
  }
};

} // unnamed namespace

namespace ssedit {

Function::Function(const Cursor& cursor, SourceFile& sourceFile)
    : Entity(cursor, sourceFile), _parameters(), _functionBody()
{
  std::vector<Cursor> parameterCursors;
  _cursor.gatherChildren(&parameterCursors, ::IsParameter());

  _parameters.reserve( parameterCursors.size() );

  std::for_each( parameterCursors.begin(), parameterCursors.end(),
                 [this](Cursor& pCursor){
                  _parameters.push_back( Parameter(pCursor, *_sourceFile) );
                 } );

  // search first compound statement, i.e. body of the function
  std::vector<Cursor> firstCompoundStmt;
  _cursor.gatherChildren(&firstCompoundStmt, ::IsFirstCompoundStmt());
  _functionBody = firstCompoundStmt.front();
}

Function::Function(const Function& rhs)
  : Entity(rhs), _parameters(rhs._parameters)
{
}

Function& Function::operator=(const Function& rhs)
{
  Entity::operator=(rhs);
  _parameters = rhs._parameters;
  return *this;
}

const std::vector<Parameter>& Function::getParameters() const
{
  return _parameters;
}

const Cursor& Function::getFunctionBody() const
{
  return _functionBody;
}

Type Function::getResultType() const
{
  return _cursor.getResultType();
}

} // namespace ssedit

