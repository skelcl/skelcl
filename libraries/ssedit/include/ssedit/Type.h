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
 * Type.h
 */

#ifndef TYPE_H_
#define TYPE_H_

#include <clang-c/Index.h>

#include <string>

//#include "Cursor.h"
#include "Type.h"

namespace ssedit {

class Cursor;

class Type {
public:
  Type();
  
  Type(CXType type);

  Type(const Type& rhs);

  Type& operator=(const Type& rhs);

  CXTypeKind getKind();

  std::string getKindSpelling();

  // TODO: Implement
//  std::string getDisplayName();

  bool isPointer();

  bool isPOD();

  bool isConstantQualified();

  Type getPointeeType();

  Type getResultType();

  Type getCanonicalType();

  Cursor getTypeDeclaration();

  CXType getCXType();

  bool isValid();

private:
  CXType _type;
};

} // namespace ssedit

#endif /* TYPE_H_ */

