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
 * Cursor.cpp
 */

#include <algorithm>
#include <sstream>
#include <vector>
#include <functional>

#include <iostream>

#include "ssedit/Cursor.h"

#include "ssedit/SourceLocation.h"
#include "ssedit/SourceRange.h"
#include "ssedit/Type.h"

namespace {

using namespace ssedit;

CXChildVisitResult findFunction(CXCursor cxCurrent,
                                CXCursor /*cxParent*/,
                                void* cData)
{
  auto data = static_cast<std::pair<Cursor&, const std::string&>*>(cData);

  Cursor current(cxCurrent);
  if (   current.isOfKind(CXCursor_FunctionDecl)
      && current.spellingIs(data->second) ) {
    data->first = current;
    return CXChildVisit_Break;
  }

  return CXChildVisit_Recurse;
}

CXChildVisitResult findTypedef(CXCursor cxCurrent,
                               CXCursor /*cxParent*/,
                               void* cData)
{
  auto data = static_cast<std::pair<Cursor&, const std::string&>*>(cData);

  Cursor current(cxCurrent);
  if (   current.isOfKind(CXCursor_TypedefDecl)
      && current.spellingIs(data->second) ) {
    data->first = current;
    return CXChildVisit_Break;
  }

  return CXChildVisit_Recurse;
}

CXChildVisitResult findCallExpressions(CXCursor cxCurrent,
                                       CXCursor /*cxParent*/,
                                       void *cData)
{
  auto data = static_cast<std::pair<std::vector<Cursor>*,
                                    const std::string&>  *>(cData);

  Cursor current(cxCurrent);
  if (   current.isOfKind(CXCursor_CallExpr)
      && current.spellingIs(data->second) ) {
    data->first->push_back(current);
  }
  return CXChildVisit_Recurse;
}

CXChildVisitResult gatherChildren(CXCursor cxCurrent,
                                  CXCursor /*cxParent*/,
                                  void *cData)
{
  auto data = static_cast<
                std::pair<std::vector<Cursor>*,
                          const std::function<bool (const Cursor&)>&>*>(cData);
  std::vector<Cursor>* vector = data->first;
  const std::function<bool (const Cursor&)>& filter = data->second;

  Cursor current(cxCurrent);
  if(filter(current)) {
    vector->push_back(current);
  }

  return CXChildVisit_Recurse;
}

CXChildVisitResult gatherImmediateChildren(CXCursor cxCurrent,
                                           CXCursor /*cxParent*/,
                                           void *cData)
{
  auto data = static_cast<
                std::pair<std::vector<Cursor>*,
                          const std::function<bool (const Cursor&)>&>*>(cData);
  std::vector<Cursor>* vector = data->first;
  const std::function<bool (const Cursor&)>& filter = data->second;

  Cursor current(cxCurrent);
  if(filter(current)) {
    vector->push_back(current);
  }

  return CXChildVisit_Break;
}

} // anonymous namespace


namespace ssedit {

Cursor::Cursor()
  : _cursor(clang_getNullCursor())
{
}

Cursor::Cursor(CXTranslationUnit tu)
  : _cursor(clang_getTranslationUnitCursor(tu))
{
}

Cursor::Cursor(CXCursor cursor)
  : _cursor(cursor)
{
}

Cursor::Cursor(const Cursor& rhs)
  : _cursor(rhs._cursor)
{
}

Cursor& Cursor::operator=(const Cursor& rhs)
{
  _cursor = rhs._cursor;
  return *this;
}

Type Cursor::getType() const
{
  return Type(clang_getCursorType(_cursor));
}

Type Cursor::getResultType() const
{
  return Type(clang_getCursorResultType(_cursor));
}

Cursor Cursor::getReferenced() const
{
  return Cursor(clang_getCursorReferenced(_cursor));
}

bool Cursor::isReferenced(const Cursor& referenced) const
{
  return clang_equalCursors(clang_getCursorReferenced(_cursor),
                            referenced._cursor);
}

CXCursor Cursor::getCXCursor() const
{
  return _cursor;
}

CXCursorKind Cursor::getKind() const
{
  return clang_getCursorKind(_cursor);
}

bool Cursor::isOfKind(CXCursorKind kind) const
{
  return (kind == getKind());
}

SourceRange Cursor::getExtent() const
{
  return SourceRange(clang_getCursorExtent(_cursor));
}

SourceLocation Cursor::getLocation() const
{
  return SourceLocation(clang_getCursorLocation(_cursor));
}

Cursor Cursor::findFunctionCursor(const std::string& identifier) const
{
  Cursor cursor;
  std::pair<Cursor&, const std::string&> data(cursor, identifier);
  clang_visitChildren(_cursor, ::findFunction, &data);
  return cursor;
}

Cursor Cursor::findTypedefCursor(const std::string& identifier) const
{
  Cursor cursor;
  std::pair<Cursor&, const std::string&> data(cursor, identifier);
  clang_visitChildren(_cursor, ::findTypedef, &data);
  return cursor;
}

void Cursor::findCallExpressionCursors(const std::string& identifier,
                                       std::vector<Cursor>* expressions) const
{
  std::pair<std::vector<Cursor>*, const std::string&> data(expressions,
                                                           identifier);
  clang_visitChildren(_cursor, ::findCallExpressions, &data);
}

void Cursor::gatherImmediateChildren(std::vector<Cursor>* vector,
                                     const std::function<bool (const Cursor&)>& filter) const
{
  if (clang_isInvalid(clang_getCursorKind(_cursor))) {
    return;
  }
  std::pair<std::vector<Cursor>*,
            const std::function<bool (const Cursor&)>&> data(vector, filter);
  clang_visitChildren(_cursor, ::gatherImmediateChildren, &data);
}

void Cursor::gatherChildren(std::vector<Cursor>* vector,
                            const std::function<bool (const Cursor&)>& filter) const
{
  if (clang_isInvalid(clang_getCursorKind(_cursor))) {
    return;
  }
  std::pair<std::vector<Cursor>*,
            const std::function<bool (const Cursor&)>&> data(vector, filter);
  clang_visitChildren(_cursor, ::gatherChildren, &data);
}

std::string Cursor::getSpelling() const
{
  CXString clString = clang_getCursorSpelling(_cursor);
  std::string spelling(clang_getCString(clString));
  clang_disposeString(clString);
  return spelling;
}

bool Cursor::spellingIs(const std::string& spelling) const
{
  CXString cxSpelling = clang_getCursorSpelling(_cursor);
  int compare = spelling.compare(clang_getCString(cxSpelling));
  clang_disposeString(cxSpelling);
  return (compare == 0);
}

bool Cursor::isNullCursor() const
{
  return clang_equalCursors(_cursor, clang_getNullCursor());
}

bool Cursor::isValid() const
{
  return !(   getKind() >= CXCursor_FirstInvalid
           && getKind() <= CXCursor_LastInvalid );
}

} // namespace ssedit

