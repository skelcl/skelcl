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
/// \file CLArgParser.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <string>
#include <stdexcept>

#include "pvsutil/CLArgParser.h"

namespace pvsutil {

CLArgParser::CLArgParser(cmdline::Description&& des,
                         cmdline::Version&& ver,
                         Logger& logger)
  : _logger(logger), _description(des), _version(ver),
    _helpArg(cmdline::Flags(cmdline::Long("help")),
             cmdline::Description("Prints this help message and then exits the program."),
             cmdline::Default(false)),
    _versionArg(cmdline::Flags(cmdline::Long("version")),
                cmdline::Description("Prints version information and then exits the program."),
                cmdline::Default(false)),
    _shortArgs(), _longArgs(), _mandatoryArgs(), _args()
{
  // registerArg(&_helpArg); does not work (??)
  this->_args.push_back(&_helpArg);
  this->_longArgs[_helpArg.longFlags().front()] = &_helpArg;
  if (!ver.getVersion().empty()) {
    this->_args.push_back(&_versionArg);
    this->_longArgs[_versionArg.longFlags().front()] = &_versionArg;
  }
}

CLArgParser::~CLArgParser()
{
}

void CLArgParser::parse(int argc, char** argv)
{
  // skip first argument (== program name)
  for (int i = 1; i < argc; ++i) {
    std::string argv_i(argv[i]);
    if (argv_i == "--") break;  // stop parsing at --

    // first char is a - => found (short or long) flag
    if (argv_i.find('-') == 0) {
      cmdline::BaseArg* arg = nullptr;
      // second char is also a - => found long flag
      if (argv_i.find('-', 1) == 1) {
        cmdline::Long key(argv_i.substr(2));
        auto iter = _longArgs.find(key);
        if (iter == _longArgs.end()) {
          throw std::invalid_argument("Argument --" + key.getName() + " invalid.");
        }
        arg = iter->second;
      } else { // found short flag
        if (argv_i.size() != 2) {
          throw std::invalid_argument("Short flags must be a single character.");
        }
        cmdline::Short key(argv_i[1]);
        auto iter = _shortArgs.find(key);
        if (iter == _shortArgs.end()) {
          throw std::invalid_argument("Argument -" + key.getName() + " invalid.");
        }
        arg = iter->second;
      }
      std::string value;
      // initialize value dependent on if flag expects an argument
      if (arg->hasArgument()) {
        value.assign(argv[i+1]); ++i;
      } else {
        value.assign("true"); // set boolean flag
      }

      arg->parse(value);
    }
  }

  if (_helpArg.getValue())    { printHelp();    }
  if (_versionArg.getValue()) { printVersion(); }

  int missingArgCount = 0;
  for (auto argPtr : _mandatoryArgs) {
    if (!argPtr->isSet()) {
      // print arg is missing
      _logger.output() << "Mandatory argument [" << argPtr->flagNames()
                       << "] is missing." << std::endl;
      ++missingArgCount;
    }
  }
  if (missingArgCount > 0) {
    throw std::invalid_argument("At least one mandatory arguments is missing. See --help for details.");
  }
}

NORETURN_ATTRIBUTE
void CLArgParser::printHelp()
{
  _logger.output() << _description.getDescription() << std::endl << std::endl;
  for (auto& argPtr : _args) {
    printArg(_logger.output(), *argPtr);
  }
  _logger.output() << std::endl;
  exit(0);
}

NORETURN_ATTRIBUTE
void CLArgParser::printVersion()
{
  _logger.output() << "Version: " << _version.getVersion() << std::endl;
  exit(0);
}

void CLArgParser::add()
{
}

void CLArgParser::registerArg(cmdline::BaseArg& arg)
{
  _args.push_back(&arg);
  
  if (arg.isMandatory()) {
    _mandatoryArgs.push_back(&arg);
  }

  auto& shortFlags = arg.shortFlags();
  for (auto& sF : shortFlags) {
    _shortArgs[sF] = &arg;
  }

  auto& longFlags = arg.longFlags();
  for (auto& lF : longFlags) {
    _longArgs[lF] = &arg;
  }
}

void CLArgParser::printArg(std::ostream& output, cmdline::BaseArg& arg)
{
  output << "\t[" << arg.flagNames() << "]";
  if (arg.isMandatory()) {
    output << " (mandatory)";
  } else { // print default
    output << " (default value: ";
    arg.printValue(output);
    output << ")";
  }
  output << "\n\t\t" << arg.description()
         << std::endl << std::endl;
}

} // namespace pvsutil

