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
/// \file Util.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

#include <cmath>
#include <cstdlib>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <openssl/sha.h>
#pragma GCC diagnostic pop

#include "SkelCL/detail/Util.h"

namespace skelcl {

namespace detail {

namespace util {

std::string envVarValue(const std::string& envVar)
{
  char* envValue = std::getenv(envVar.c_str());
  if (envValue != NULL) {
    return envValue;
  } else {
    return "";
  }
}

std::string hash(const std::string& string)
{
  unsigned char raw[20];
  char* c_str = const_cast<char*>(string.c_str());
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  SHA1(reinterpret_cast<unsigned char*>(c_str), string.length(), raw);
#pragma GCC diagnostic pop
  std::ostringstream buffer;
  for (int i = 0; i < 20; ++i) {
    buffer << std::hex
           << std::setw(2)
           << std::setfill('0')
           << static_cast<int>( raw[i] );
  }
  return buffer.str();
}

int devideAndRoundUp(int i, int j)
{
  int r = i / j;
  if (i % j != 0) {
    r++;
  }
  return r;
}

int devideAndAlign(int i, int j, int a)
{
  int x = i / j;
  if (i % j != 0)
    ++x;
  int r = x % a;
  if (r != 0)
    x = x + (a - r);
  return x;
}

int ceilToMultipleOf(int i, int j)
{
  int r = i % j;
  if (r == 0)
   return i;
  else
   return i + (j - r);
}

bool isPowerOfTwo(int n)
{
  return ((n & (n - 1)) == 0);
}

int floorPow2(int n)
{
  int exp;
  frexp(static_cast<float>(n), &exp);
  return 1 << (exp - 1);
}

int ceilPow2(int n)
{
  int exp;
  frexp(static_cast<float>(n), &exp);
  return 1 << exp;
}

} // namespace util

} // namespace detail

} // namespace skelcl
