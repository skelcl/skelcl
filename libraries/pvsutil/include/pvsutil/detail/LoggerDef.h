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
/// \file LoggerDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef LOGGER_DEF_H_
#define LOGGER_DEF_H_

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include "../Assert.h"
#include "pvsutilDll.h"

namespace pvsutil {

namespace logger_impl {

PVSUTIL_API std::string getErrorString(cl_int err);

PVSUTIL_API std::string formatHeader(const Logger& logger,
                                     Logger::Severity::Type severity,
                                     const char*    file,
                                     const int      line);

} // namespace logger_impl

template <typename... Args>
void Logger::log(Severity::Type severity, const char* file, int line,
                 Args&&... args)
{
  if (severity <= _severity) {
    *_output << logger_impl::formatHeader(*this, severity, file, line);
    logArgs(*_output, std::forward<Args>(args)...);
  }
}

template <typename... Args>
void Logger::logArgs(std::ostream& output, const cl::Error& err,
                     Args&&... args)
{
  output << "OpenCL error: " << logger_impl::getErrorString(err.err())
         << " " << err.what();
  logArgs(output, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
void Logger::logArgs(std::ostream& output, T value, Args&&... args)
{
  output << value;
  logArgs(output, std::forward<Args>(args)...);
}

} // namespace pvsutil

#endif // LOGGER_DEF_H_

