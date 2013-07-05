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
/// \file Flags.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef FLAGS_H_
#define FLAGS_H_

#include <string>
#include <vector>

#include "../detail/pvsutilDll.h"

namespace pvsutil {

namespace cmdline {

class PVSUTIL_API Short {
public:
  Short(const char c);

  bool operator<(const Short& rhs) const;

  const std::string& getName() const;
private:
  std::string _name;
};

class PVSUTIL_API Long {
public:
  Long(const char* name);
  Long(const std::string& name);

  bool operator<(const Long& rhs) const;

  const std::string& getName() const;
private:
  std::string _name;
};

class PVSUTIL_API Flags {
public:
  template <class... Args>
  Flags(const Short& shortFlag, Args... args);

  template <class... Args>
  Flags(const Long& longFlag, Args... args);

  const std::vector<Short>& getShortFlags() const;
  const std::vector<Long>&  getLongFlags()  const;
private:
  void init();

  template <class... Args>
  void init(const Short& shortFlag, Args... args);

  template <class... Args>
  void init(const Long& longFlag, Args... args);

  std::vector<Short> _shortFlags;
  std::vector<Long>  _longFlags;
};

template <class... Args>
Flags::Flags(const Short& shortFlag, Args... args)
  : _shortFlags(), _longFlags()
{
  this->init(shortFlag, std::forward<Args>(args)...);
}

template <class... Args>
Flags::Flags(const Long& longFlag, Args... args)
  : _shortFlags(), _longFlags()
{
  this->init(longFlag, std::forward<Args>(args)...);
}

template <class... Args>
void Flags::init(const Short& shortFlag, Args... args)
{
  _shortFlags.push_back(shortFlag);
  this->init(std::forward<Args>(args)...);
}

template <class... Args>
void Flags::init(const Long& longFlag, Args... args)
{
  _longFlags.push_back(longFlag);
  this->init(std::forward<Args>(args)...);
}

} // namespace cmdline

} // namespace pvsutil

#endif // FLAGS_H_

