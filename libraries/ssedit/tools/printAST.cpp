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

// printAST.cpp

#include <clang-c/Index.h>

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
CXTranslationUnit tu;
struct Cursor {
  Cursor(CXCursor cxCursor)
    : _cxCursor(cxCursor), _children() {
  };
  CXCursor _cxCursor;
  std::vector<std::shared_ptr<Cursor>> _children;
};

enum CXChildVisitResult GatherCursor(CXCursor cxCursor,
                                     CXCursor /*cxParent*/,
                                     CXClientData cxClientData) {
  if ( clang_isInvalid(cxCursor.kind) )
    return CXChildVisit_Continue; // skip
//  if ( clang_isUnexposed(cxCursor.kind) )
//    return CXChildVisit_Continue; // skip
  std::shared_ptr<Cursor>* parent = static_cast<std::shared_ptr<Cursor>*>(cxClientData);
  auto cursor = std::make_shared<Cursor>(cxCursor);
  (*parent)->_children.push_back(cursor);
  return CXChildVisit_Continue; // Not traverse the tree down here
}

void gatherChildren(std::shared_ptr<Cursor> root) {
  clang_visitChildren(root->_cxCursor, GatherCursor, &root);

  std::for_each(root->_children.begin(), root->_children.end(),
                gatherChildren);
}

void printChildren(std::shared_ptr<Cursor> root, const unsigned int level) {
  for (unsigned int i=0; i < level; ++i)
    std::cout << "  ";

  unsigned int line, column, offset;
  clang_getSpellingLocation(clang_getCursorLocation(root->_cxCursor), NULL, &line, &column, &offset);
  CXString cxSpelling         = clang_getCursorSpelling(root->_cxCursor);
  CXString cxKindSpelling     = clang_getCursorKindSpelling(root->_cxCursor.kind);
  CXString cxTypeKindSpelling = clang_getTypeKindSpelling(clang_getCursorType(root->_cxCursor).kind);
  CXSourceRange cxRange       = clang_getCursorExtent(root->_cxCursor);
  unsigned int sLine, sColumn, sOffset;
  unsigned int eLine, eColumn, eOffset;
  clang_getSpellingLocation(clang_getRangeStart(cxRange), NULL, &sLine, &sColumn, &sOffset);
  clang_getSpellingLocation(clang_getRangeEnd(cxRange), NULL, &eLine, &eColumn, &eOffset);


  std::cout << clang_getCString(cxSpelling)
   << " : " << clang_getCString(cxKindSpelling)
   << " : " << clang_getCString(cxTypeKindSpelling)
   << " : " << line << " " << column << " " << offset
   << " : " <<sLine << " " <<sColumn << " " <<sOffset
   << " - " <<eLine << " " <<eColumn << " " <<eOffset;

  clang_getInstantiationLocation(clang_getRangeStart(cxRange), NULL, &sLine, &sColumn, &sOffset);
  clang_getInstantiationLocation(clang_getRangeEnd(cxRange), NULL, &eLine, &eColumn, &eOffset);

  std::cout << " expansion: " << sLine << " " << sColumn << " " << sOffset
            << " - "        << eLine << " " << eColumn << " " << eOffset;

  if ( clang_getCursorKind(root->_cxCursor) == CXCursor_CallExpr ) {
  }

  if (root->_cxCursor.kind == CXCursor_TypedefDecl) {
    CXString cxCanonicalTypeKindSpelling = clang_getTypeKindSpelling(clang_getCanonicalType(clang_getCursorType(root->_cxCursor)).kind);
    std::cout << "CAN: " << clang_getCString(cxCanonicalTypeKindSpelling);
//    CXCursor decl = clang_getTypeDeclaration(clang_getCursorType(root->_cxCursor));
//    unsigned int decLine, decColumn, decOffset;
//    clang_getSpellingLocation(clang_getCursorLocation(decl), NULL, &decLine, &decColumn, &decOffset);
//    std::cout << "DECL: " << decLine << " " << decColumn << " " << decOffset << " ";
    clang_disposeString(cxCanonicalTypeKindSpelling);
  }
  std::cout << std::endl;

  clang_disposeString(cxTypeKindSpelling);
  clang_disposeString(cxKindSpelling);
  clang_disposeString(cxSpelling);

  std::for_each(root->_children.begin(), root->_children.end(),
      [level](std::shared_ptr<Cursor> child) {
        printChildren(child, level+1);
      } );
}

int main(int argc, const char * const * argv) {
  CXIndex index = clang_createIndex(0,0);
  tu = clang_parseTranslationUnit(index, NULL, argv, argc, NULL, 0, CXTranslationUnit_None);

  CXCursor cxCursor = clang_getTranslationUnitCursor(tu);

  auto root = std::make_shared<Cursor>(cxCursor);

  gatherChildren(root);

  printChildren(root, 0);

  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);
}

