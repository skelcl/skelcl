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
/// \file Arg.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef ARG_H_
#define ARG_H_

#include <functional>
#include <sstream>
#include <stdexcept>

#include "Description.h"
#include "Version.h"
#include "Flags.h"
#include "Default.h"

namespace pvsutil {

namespace cmdline {

namespace helper {

template <class T>
struct hasArgument { const static bool value; };

template <class T>
const bool hasArgument<T>::value = true;

template <>
const bool hasArgument<bool>::value;
// initialization of hasArgument<bool>::value as false in .cpp file

}

class BaseArg {
public:
  const std::vector<Short>& shortFlags() const;
  const std::vector<Long>&   longFlags() const;
  std::string flagNames() const;
  const std::string& description() const;
  bool isMandatory() const;
  bool isSet() const;

  virtual void parse(const std::string& value) = 0;
  virtual bool hasArgument() const = 0;
  virtual void printValue(std::ostream& output) const = 0;

  virtual ~BaseArg() {}
protected:
  BaseArg(const Flags& flags, const Description& des, bool mandatory, bool set);

  Flags                         _flags;
  Description                   _description;
  bool                          _mandatory;
  bool                          _set;
};


template <typename T>
class ArgImpl : public BaseArg {
public:
  ArgImpl(const Flags& flags,
          const Description& des)
    : BaseArg(flags, des, true, false), _value()
  {
  }

  ArgImpl(const Flags& flags,
          const Description& des,
          const DefaultValue<T>& defaultValue)
    : BaseArg(flags, des, false, true),
      _value(defaultValue.getValue())
  {
  }

  void parse(const std::string& value)
  {
    std::istringstream is(value);
    is >> std::boolalpha >> _value;
    if (is.fail()) {
      throw std::invalid_argument("Could not parse argument: "
                                  + _description.getDescription()
                                  + " for given value: "
                                  + value);
    }
    _set = true;
  }

  bool hasArgument() const
  {
    return helper::hasArgument<T>::value;
  }

  void printValue(std::ostream& output) const
  {
    output << std::boolalpha << _value;
  }

  const T& getValue() const
  {
    return _value;
  }

  operator T() const
  {
    return _value;
  }

private:
  T                             _value;
};

template <typename T>
class ArgCustomParseFunctionImpl : public BaseArg {
public:
  ArgCustomParseFunctionImpl(const Flags& flags,
                             const Description& des,
                             std::function<T(const std::string&)> parseFunc)
    : BaseArg(flags, des, true, false), _value(), _parseFunc(parseFunc)
  {
  }

  ArgCustomParseFunctionImpl(const Flags& flags,
                             const Description& des,
                             const DefaultValue<T>& defaultValue,
                             std::function<T(const std::string&)> parseFunc)
    : BaseArg(flags, des, false, true),
      _value(defaultValue.getValue()), _parseFunc(parseFunc)
  {
  }

  void parse(const std::string& value)
  {
    _value = _parseFunc(value);
    _set = true;
  }

  bool hasArgument() const
  {
    return helper::hasArgument<T>::value;
  }

  void printValue(std::ostream& output) const
  {
    output << std::boolalpha << _value;
  }

  const T& getValue() const
  {
    return _value;
  }

  operator T() const
  {
    return _value;
  }

private:
  T                                     _value;
  std::function<T(const std::string&)>  _parseFunc;
};

template <typename T>
ArgImpl<T> Arg(const Flags& flags,
               const Description& des)
{
  return ArgImpl<T>(flags, des);
}

template <typename T>
ArgImpl<T> Arg(const Flags& flags,
               const Description& des,
               const DefaultValue<T>& defaultValue)
{
  return ArgImpl<T>(flags, des, defaultValue);
}

template <typename T>
ArgCustomParseFunctionImpl<T> Arg(const Flags& flags,
                                  const Description& des,
                                  std::function<T(const std::string&)> parseFunc)
{
  return ArgCustomParseFunctionImpl<T>(flags, des, parseFunc);
}

template <typename T>
ArgCustomParseFunctionImpl<T> Arg(const Flags& flags,
                                  const Description& des,
                                  const DefaultValue<T>& defaultValue,
                                  std::function<T(const std::string&)> parseFunc)
{
  return ArgCustomParseFunctionImpl<T>(flags, des, defaultValue, parseFunc);
}

} // namespace cmdline

} // namespace pvsutil

#endif // ARG_H_

