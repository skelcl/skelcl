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

#ifndef TYPES_H_
#define TYPES_H_

namespace skelcl {

#define _DEF_TYPE_2(type, name) \
  typedef struct {\
    type s0;\
    type s1;\
  } name##2;

#define _DEF_TYPE_4(type, name) \
  typedef struct {\
    type s0;\
    type s1;\
    type s2;\
    type s3;\
  } name##4;

#define _DEF_TYPE_8(type, name) \
  typedef struct {\
    type s0;\
    type s1;\
    type s2;\
    type s3;\
    type s4;\
    type s5;\
    type s6;\
    type s7;\
  } name##8;

#define _DEF_TYPE_16(type, name) \
  typedef struct {\
    type s0;\
    type s1;\
    type s2;\
    type s3;\
    type s4;\
    type s5;\
    type s6;\
    type s7;\
    type s8;\
    type s9;\
    type sa;\
    type sb;\
    type sc;\
    type sd;\
    type se;\
    type sf;\
  } name##16;

_DEF_TYPE_2(char, char);
_DEF_TYPE_4(char, char);
typedef char4 char3;
_DEF_TYPE_8(char, char);
_DEF_TYPE_16(char, char);

_DEF_TYPE_2(unsigned char, uchar);
_DEF_TYPE_4(unsigned char, uchar);
typedef uchar4 uchar3;
_DEF_TYPE_8(unsigned char, uchar);
_DEF_TYPE_16(unsigned char, uchar);

_DEF_TYPE_2(short, short);
_DEF_TYPE_4(short, short);
typedef short4 short3;
_DEF_TYPE_8(short, short);
_DEF_TYPE_16(short, short);

_DEF_TYPE_2(unsigned short, ushort);
_DEF_TYPE_4(unsigned short, ushort);
typedef ushort4 ushort3;
_DEF_TYPE_8(unsigned short, ushort);
_DEF_TYPE_16(unsigned short, ushort);

_DEF_TYPE_2(int, int);
_DEF_TYPE_4(int, int);
typedef int4 int3;
_DEF_TYPE_8(int, int);
_DEF_TYPE_16(int, int);

_DEF_TYPE_2(unsigned int, uint);
_DEF_TYPE_4(unsigned int, uint);
typedef uint4 uint3;
_DEF_TYPE_8(unsigned int, uint);
_DEF_TYPE_16(unsigned int, uint);

_DEF_TYPE_2(long, long);
_DEF_TYPE_4(long, long);
typedef long4 long3;
_DEF_TYPE_8(long, long);
_DEF_TYPE_16(long, long);

_DEF_TYPE_2(unsigned long, ulong);
_DEF_TYPE_4(unsigned long, ulong);
typedef ulong4 ulong3;
_DEF_TYPE_8(unsigned long, ulong);
_DEF_TYPE_16(unsigned long, ulong);

_DEF_TYPE_2(float, float);
_DEF_TYPE_4(float, float);
typedef float4 float3;
_DEF_TYPE_8(float, float);
_DEF_TYPE_16(float, float);

_DEF_TYPE_2(double, double);
_DEF_TYPE_4(double, double);
typedef double4 double3;
_DEF_TYPE_8(double, double);
_DEF_TYPE_16(double, double);

#undef _DEF_TYPE_2
#undef _DEF_TYPE_4
#undef _DEF_TYPE_8
#undef _DEF_TYPE_16

} // namespace skelcl

#endif // TYPES_H_

