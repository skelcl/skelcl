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
/// \file CLArgParser.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef CLARGPARSER_H_
#define CLARGPARSER_H_

#include <ostream>
#include <string>
#include <map>

#include "Logger.h"

#include "cmdline/Arg.h"
#include "cmdline/Description.h"
#include "cmdline/Version.h"

#include "detail/pvsutilDll.h"

#ifdef __clang__
# define NORETURN_ATTRIBUTE [[ noreturn ]]
#else
# ifdef __GNUC__
#   define NORETURN_ATTRIBUTE __attribute__((noreturn))
# else
#   define NORETURN_ATTRIBUTE
# endif
#endif

namespace pvsutil {

class PVSUTIL_API CLArgParser {
public:
  CLArgParser(cmdline::Description&& des,
              cmdline::Version&& ver = cmdline::Version(),
              Logger& logger = defaultLogger);

  ~CLArgParser();

  // add multiple Arg<T> to the parser
  template <class... Args>
  void add(cmdline::BaseArg* arg, Args... args);

  void parse(int argc, char** argv);

private:

  void add();

  void registerArg(cmdline::BaseArg& arg);

  void printArg(std::ostream& output, cmdline::BaseArg& arg);

  void printDescription();

  NORETURN_ATTRIBUTE
  void printHelp();

  NORETURN_ATTRIBUTE
  void printVersion();


  Logger&                         _logger;
  cmdline::Description            _description;
  cmdline::Version                _version;

  cmdline::ArgImpl<bool>          _helpArg;
  cmdline::ArgImpl<bool>          _versionArg;

  std::map<cmdline::Short,
           cmdline::BaseArg*>     _shortArgs;
  std::map<cmdline::Long,
           cmdline::BaseArg*>     _longArgs;
  std::vector<cmdline::BaseArg*>  _mandatoryArgs;
  std::vector<cmdline::BaseArg*>  _args;
};

template <class... Args>
void CLArgParser::add(cmdline::BaseArg* arg, Args... args)
{
  this->registerArg(*arg);
  add(std::forward<Args>(args)...);
}

} // namespace pvsutil

#endif // PARSER_H_

