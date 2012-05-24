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
 * Type.cpp
 */

#include <algorithm>
#include <string>
#include <sstream>

#include <clang-c/Index.h>

#include "ssedit/Type.h"
#include "ssedit/Cursor.h"

namespace ssedit {

Type::Type()
  : _type() {
}

Type::Type(CXType type)
  : _type(type) {
}

Type::Type(const Type& rhs)
  : _type(rhs._type) {
}

Type& Type::operator=(const Type& rhs) {
  _type = rhs._type;
  return *this;
}

CXTypeKind Type::getKind() {
  return _type.kind;
}

std::string Type::getKindSpelling() {
  CXString cxSpelling = clang_getTypeKindSpelling(_type.kind);
  std::string spelling = clang_getCString(cxSpelling);
  clang_disposeString(cxSpelling);
  return spelling;
}

/*
std::string Type::getDisplayName() {
  if(getKind() != CXType_Unexposed) {
    if(!isPointer()) {
      if(isPOD() || getKind() == CXType_Void) {
        std::string tmp = getKindSpelling();
        std::transform(tmp.begin(), tmp.end(),tmp.begin(), ::tolower); // TODO: ?!
        return tmp;
      }else if(getKind() == CXType_LValueReference){
        return getPointeeType().getDisplayName().append("&");
      }
    } else {
      Type pointeeType = getPointeeType();
      return pointeeType.getDisplayName().append("*");
    }
  }
  //The type is a custom type. We check for namespaces and add them as a prefix
  // to the type-string, which is returned by this function
  std::ostringstream oss;
  Cursor typeDecl = getTypeDeclaration();
    for(Cursor sema = typeDecl.getSemanticParent(); sema.getKind() == CXCursor_Namespace; sema = sema.getSemanticParent()) {
      oss << sema.getDisplayName() << "::";
    }
  oss << typeDecl.getDisplayName();
  return oss.str();
}
*/

bool Type::isPointer() {
	return (_type.kind == CXType_Pointer);
}

bool Type::isConstantQualified() {
	return clang_isConstQualifiedType(_type);
}

bool Type::isPOD() {
	return clang_isPODType(_type);
}

Type Type::getPointeeType() {
	return clang_getPointeeType(_type);
}

Type Type::getResultType() {
	return clang_getResultType(_type);
}

Type Type::getCanonicalType() {
	return clang_getCanonicalType(_type);
}

Cursor Type::getTypeDeclaration() {
	return Cursor(clang_getTypeDeclaration(_type));
}

CXType Type::getCXType() {
  return _type;
}

bool Type::isValid() {
  return (_type.kind != CXType_Invalid);
}

} // namespace ssedit

