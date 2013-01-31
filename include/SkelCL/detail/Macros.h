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

#ifndef MACRO_H_
#define MACRO_H_

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)

#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE \
  CONCATENATE(anonymous, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE \
  CONCATENATE(anonymous, __LINE__)
#endif

#define SKELCL_COMMON_DEFINITION(definition) \
  definition \
  skelcl::detail::RegisterCommonDefinition ANONYMOUS_VARIABLE(#definition);

#define SKELCL_ADD_DEFINE(define) \
  skelcl::detail::RegisterCommonMacroDefinition \
    ANONYMOUS_VARIABLE(#define, define);

#define FULLY_EXPANDED(count, ...) \
  SKELCL_ADD_DEFINE_##count(__VA_ARGS__)

#define SEMI_EXPANDED(count, ...) \
  FULLY_EXPANDED(count, __VA_ARGS__)

#define SKELCL_ADD_DEFINES(...) \
  SEMI_EXPANDED(VA_NARGS(__VA_ARGS__), __VA_ARGS__)

#define SKELCL_ADD_DEFINE_1(a) SKELCL_ADD_DEFINE(a)
#define SKELCL_ADD_DEFINE_2(a,b) \
  SKELCL_ADD_DEFINE_1(a) SKELCL_ADD_DEFINE(b)
#define SKELCL_ADD_DEFINE_3(a,b,c) \
  SKELCL_ADD_DEFINE_2(a,b) SKELCL_ADD_DEFINE(c)
#define SKELCL_ADD_DEFINE_4(a,b,c,d) \
  SKELCL_ADD_DEFINE_3(a,b,c) SKELCL_ADD_DEFINE(d)
#define SKELCL_ADD_DEFINE_5(a,b,c,d,e) \
  SKELCL_ADD_DEFINE_4(a,b,c,d) SKELCL_ADD_DEFINE(e)
#define SKELCL_ADD_DEFINE_6(a,b,c,d,e,f) \
  SKELCL_ADD_DEFINE_5(a,b,c,d,e) SKELCL_ADD_DEFINE(f)
#define SKELCL_ADD_DEFINE_7(a,b,c,d,e,f,g) \
  SKELCL_ADD_DEFINE_6(a,b,c,d,e,f) SKELCL_ADD_DEFINE(g)
#define SKELCL_ADD_DEFINE_8(a,b,c,d,e,f,g,h) \
  SKELCL_ADD_DEFINE_7(a,b,c,d,e,f,g) SKELCL_ADD_DEFINE(h)
#define SKELCL_ADD_DEFINE_9(a,b,c,d,e,f,g,h,i) \
  SKELCL_ADD_DEFINE_8(a,b,c,d,e,f,g,h) SKELCL_ADD_DEFINE(i)

#endif // MACRO_H_

