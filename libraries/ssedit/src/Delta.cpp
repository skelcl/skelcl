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
 * Delta.cpp
 */

#include <clang-c/Index.h>

#include <memory>
#include <string>

#include "ssedit/Delta.h"

#include "ssedit/SourceLocation.h"

namespace ssedit {

Delta::Delta(const SourceRange& range,
             const std::string& newToken,
             Delta::Kind kind)
  : _range(range), _newToken(newToken),
    _kind(kind), _next()
{
}

const std::string& Delta::getNewToken() const {
  return _newToken;
}

void Delta::setNewToken(const std::string& newToken) {
  _newToken = newToken;
}

Delta::Kind Delta::getKind() const {
  return _kind;
}

std::shared_ptr<Delta> Delta::getNextDelta() const {
  return _next;
}

void Delta::setNextDelta(std::shared_ptr<Delta> next) {
  _next = next;
}

SourceRange Delta::getRange() const {
  return _range;
}

} // namespace ssedit

