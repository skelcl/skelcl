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
 * TempSourceFile.cpp
 */

#include <string>
#include <fstream>

#include <cstdio>

#include "ssedit/TempSourceFile.h"
#include "ssedit/SourceFile.h"

namespace {

std::string createTmpFile(const std::string& content,
                          const std::string& tmpFileName)
{
  std::ofstream tmpFile(tmpFileName, std::ios_base::trunc);
  { // TODO: remove when libclang is able to parse OpenCL C code
    std::string openCLFix("\
#define __kernel\n\
#define kernel\n\
#define __global\n\
#define global\n\
#define __local\n\
#define local\n\
#define __constant\n\
#define constant\n\
#define __private\n\
#define private\n\
/*!!*/\n");
    tmpFile.write(openCLFix.c_str(), openCLFix.size());
  }
  tmpFile.write(content.c_str(), content.size());
  return tmpFileName;
}

} // unnamed namespace

namespace ssedit {

TempSourceFile::TempSourceFile(const std::string& content,
                               const std::string& tmpFileName)
  : SourceFile(::createTmpFile(content, tmpFileName)), _tmpFileName(tmpFileName)
{
}

TempSourceFile::TempSourceFile(const TempSourceFile& rhs)
  : SourceFile(rhs), _tmpFileName(rhs._tmpFileName)
{
}

TempSourceFile::~TempSourceFile()
{
  // call C function to remove tmp file
  remove(_tmpFileName.c_str());
}

} // namespace ssedit

