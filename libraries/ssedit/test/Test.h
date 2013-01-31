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
 
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

#include <gtest/gtest.h>

#pragma GCC diagnostic pop

#include <clang-c/Index.h>

#include <fstream>
#include <string>

struct TranslationUnit {
  TranslationUnit(const std::string& source)
    : _fileName("tmpSource.c"), _index(), _tu() {
    std::ofstream tmpFile(_fileName, std::ios_base::trunc);
    tmpFile.write(source.c_str(), source.size());

    _index = clang_createIndex(0, 0);
    _tu = clang_parseTranslationUnit(_index, _fileName.c_str(), NULL, 0, NULL, 0, CXTranslationUnit_None);
  };

  ~TranslationUnit() {
    remove(_fileName.c_str());
  };

  std::string _fileName;
  CXIndex _index;
  CXTranslationUnit _tu;
};

