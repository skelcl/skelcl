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
/// \file Assert.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef ASSERT_H_
#define ASSERT_H_

#include "detail/pvsutilDll.h"

#ifdef NDEBUG

#define ASSERT(e) (void(0))
#define ASSERT_MESSAGE(e, ...) (void(0))
#define ONLY_IN_DEBUG(e) (void(0))

#else  // DEBUG

#define ASSERT(e)\
  pvsutil::assert_impl::ASSERT_IMPL(__FILE__, __LINE__, e, #e)

#define ASSERT_MESSAGE(e, ...)\
  pvsutil::assert_impl::ASSERT_IMPL(__FILE__, __LINE__, e, #e, __VA_ARGS__)

#define ONLY_IN_DEBUG(e) e

#endif // NDEBUG

namespace pvsutil {

namespace assert_impl {

///
/// \brief If expression evaluates to false an error message is printed and the
///        execution is aborted. If expression evaluates to true the function
///        call has no effect.
///
/// \param file A string naming the file the functions was called in. Used as
///             part of the printed the error message.
///
///        line An integer naming the line the functions was called in. Used as
///             part of the printed the error message.
///
///        expression The expression to be evaluated.
///
///        expressionString The expression as a string to be printed as part of
///                         the error message
///
PVSUTIL_API void ASSERT_IMPL(const char* file,
                             const int   line,
                             const bool  expression,
                             const char* expressionString);

///
/// \brief If expression evaluates to false an error message is printed and the
///        execution is aborted. If expression evaluates to true the function
///        call has no effect.
///
/// \param file A string naming the file the functions was called in. Used as
///             part of the printed the error message.
///
///        line An integer naming the line the functions was called in. Used as
///             part of the printed the error message.
///
///        expression The expression to be evaluated.
///
///        expressionString The expression as a string to be printed as part of
///                         the error message
///
///        formatString A formated String to be printed as part of the error
///                     message. The String is evaluated by vsnprintf.
///
PVSUTIL_API void ASSERT_IMPL(const char* file,
                             const int   line,
                             const bool  expression,
                             const char* expressionString,
                             const char* formatString, ...);

} // namespace assert_impl

} // namespace pvsutil

#endif // ASSERT_H_

