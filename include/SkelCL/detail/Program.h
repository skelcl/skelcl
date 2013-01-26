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
/// \file Progam.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <string>
#include <map>
#include <memory>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include <ssedit/TempSourceFile.h>

#include "Device.h"
#include "Util.h"

namespace skelcl {

namespace detail {

class Program {
public:
  Program() = delete;

  Program(const std::string& source, const std::string& hash = "");

  Program(const Program&) = default;

  Program(Program&&);

  Program& operator=(const Program&) = default;

  Program& operator=(Program&&);

  ~Program() = default;

  void transferParameters(const std::string& from,
                          unsigned indexFrom,
                          const std::string& to);

  void transferArguments(const std::string& from,
                         unsigned indexFrom,
                         const std::string& to);

  void renameFunction(const std::string& from, const std::string& to);

  template<typename Head, typename ...Tail>
  void adjustTypes();

  bool loadBinary();

  void build();

  cl::Kernel kernel(const Device& device, const std::string& name) const;

private:
  void createProgramsFromSource();

  void saveBinary();

  void renameType(const int i, const std::string& name);

  template<typename T>
  void traverseTypes(int i);

  template<typename Head, typename Second, typename ...Tail>
  void traverseTypes(int i);

  std::shared_ptr<ssedit::TempSourceFile> _sourceFile;
  std::string                             _hash;
  std::vector<cl::Program>                _clPrograms;
};

// function template definitions

template<typename Head, typename... Tail>
void Program::adjustTypes() {
  traverseTypes<Head, Tail...>(0);
}

template<typename T>
void Program::traverseTypes(int i) {
  renameType(i, util::typeToString<T>());
}

template<typename Head, typename Second, typename... Tail>
void Program::traverseTypes(int i) {
  renameType(i, util::typeToString<Head>());
  traverseTypes<Second, Tail...>(++i);
}

} // namespace detail

} // namespace skelcl

#endif // PROGRAM_H_

