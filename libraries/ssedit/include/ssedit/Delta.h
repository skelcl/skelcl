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
 * Delta.h
 */

#ifndef DELTA_H_
#define DELTA_H_

#include <clang-c/Index.h>

#include <memory>
#include <string>

#include "SourceRange.h"

namespace ssedit {

class Delta {
public:
  enum Kind {
    REPLACE,
    INSERT
  };

  Delta() = default;

  Delta(const Delta&) = default;

  Delta(const SourceRange& range, const std::string& newToken, Kind kind);

  Delta& operator=(const Delta&) = default;

  ~Delta() = default;

  const std::string& getNewToken() const;

  void setNewToken(const std::string& newToken);

  Kind getKind() const;

  std::shared_ptr<Delta> getNextDelta() const;

  void setNextDelta(std::shared_ptr<Delta> next);

  SourceRange getRange() const;

private:

  SourceRange             _range;
  std::string             _newToken;
  Kind                    _kind;
  std::shared_ptr<Delta>  _next;
};

} // namespace ssedit

#endif /* DELTA_H_ */

