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
/// \file Assert.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <cstdarg>
#include <cstdio>
#include <memory>

#include "pvsutil/Assert.h"

#include "pvsutil/Logger.h"

namespace pvsutil {

namespace assert_impl {

void ASSERT_IMPL(const char* file,
                 const int   line,
                 const bool  expression,
                 const char* expressionString)
{
  if (!expression) {
    defaultLogger.log(Logger::Severity::Error, file, line,
                      "Assertion `", expressionString,
                      "' failed.");
    abort();
  }
}

#ifdef _MSC_VER
#pragma warning(disable:4996)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

void ASSERT_IMPL(const char* file,
                 const int   line,
                 const bool  expression,
                 const char* expressionString,
                 const char* formatString, ...)
{
  if (!expression) {
    va_list args;
    va_start(args, formatString);
    auto needed = vsnprintf(NULL, 0, formatString, args) + 1;
    ASSERT(needed > 0);
    {
      std::unique_ptr<char[]> buffer(new char[needed]);

      vsnprintf(&buffer[0], static_cast<size_t>(needed), formatString, args);

      defaultLogger.log(Logger::Severity::Error, file, line,
                        "Assertion `", expressionString, "' failed. ",
                        &buffer[0]);
    }
    va_end(args);
    abort();
  }
}

#ifdef _MSC_VER
#pragma warning(default:4996)
#else
#pragma GCC diagnostic pop
#endif

} // namespace assert_impl

} // namespace pvsutil

