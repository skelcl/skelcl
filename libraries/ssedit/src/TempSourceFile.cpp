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

#include <pvsutil/Logger.h>

#include "ssedit/TempSourceFile.h"
#include "ssedit/SourceFile.h"

namespace {

std::string createTmpFile(const std::string& content,
                          const std::string& tmpFileName)
{
  std::ofstream tmpFile(tmpFileName, std::ios_base::trunc);
  { // TODO: remove when libclang is able to parse OpenCL C code
    std::string openCLFix("\
/* OpenCL fix start */\n\
#define char2 char\n\
#define char3 char\n\
#define char4 char\n\
#define char8 char\n\
#define char16 char\n\
#define uchar2 uchar\n\
#define uchar3 uchar\n\
#define uchar4 uchar\n\
#define uchar8 uchar\n\
#define uchar16 uchar\n\
#define short2 short\n\
#define short3 short\n\
#define short4 short\n\
#define short8 short\n\
#define short16 short\n\
#define ushort2 ushort\n\
#define ushort3 ushort\n\
#define ushort4 ushort\n\
#define ushort8 ushort\n\
#define ushort16 ushort\n\
#define int2 int\n\
#define int3 int\n\
#define int4 int\n\
#define int8 int\n\
#define int16 int\n\
#define uint2 uint\n\
#define uint3 uint\n\
#define uint4 uint\n\
#define uint8 uint\n\
#define uint16 uint\n\
#define long2 long\n\
#define long3 long\n\
#define long4 long\n\
#define long8 long\n\
#define long16 long\n\
#define ulong2 ulong\n\
#define ulong3 ulong\n\
#define ulong4 ulong\n\
#define ulong8 ulong\n\
#define ulong16 ulong\n\
#define float2 float\n\
#define float3 float\n\
#define float4 float\n\
#define float8 float\n\
#define float16 float\n\
#define double2 double\n\
#define double3 double\n\
#define double4 double\n\
#define double8 double\n\
#define double16 double\n\
/* OpenCL fix end */\n");
    tmpFile.write(openCLFix.c_str(), static_cast<long>(openCLFix.size()));
  }
  tmpFile.write(content.c_str(), static_cast<long>(content.size()));
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

// TODO: remove when libclang is able to parse OpenCL C code
void TempSourceFile::removeOpenCLFix()
{
  std::string source;
  {
    std::ifstream file(_tmpFileName);
    // read content into a string
    source.assign( (std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>() );
  }

  auto endOfFix = source.find("/* OpenCL fix end */");
  source.erase( 0, endOfFix );

  std::ofstream file(_tmpFileName, std::ios_base::trunc);
  file.write(source.c_str(), static_cast<long>(source.size()));
}

} // namespace ssedit

