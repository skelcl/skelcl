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
/// \file Source.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef SOURCE_H_
#define SOURCE_H_

#include <istream>
#include <string>
#include <sstream>
#include <vector>

namespace skelcl {

///
/// \class Source
///
/// \brief This class is a unified wrapper for defining source code in SkelCL
///
/// An instance of the class can be constructed by providing a string 
/// containing the source code or an istream, from which the content it is 
/// pointing to  is interpreted to be the source code.
///
class Source {
public:
  Source();

  ///
  /// \brief Constructor taking the source code as a string
  ///
  Source(const char* source);

  ///
  /// \brief Constructor taking the source code as a string
  ///
  Source(const std::string& source);

  ///
  /// \brief Constructor taking a istream object. The content the istream is
  ///        pointing to is read using the stream operator and interpreted to
  ///        be the source code.
  ///        This variant can be used to read the source code from a file.
  ///
  Source(std::istream& is);

  ///
  /// \brief Constructor taking a istream object. The content the istream is
  ///        pointing to is read using the stream operator and interpreted to
  ///        be the source code.
  ///        This variant can be used to read the source code from a file.
  ///
  Source(std::istream&& is);

  ///
  /// \brief Default destructor
  ///
  ~Source();

  ///
  /// \brief Conversion operator providing access to the source code as a 
  ///        string
  ///
  /// Thanks to this operator an instance of this class is implicitly 
  /// convertible to a string.
  ///
  operator std::string() const;

  void append(const std::string& source);

private:
  /// string used to store the source code
  std::string _source;
};

namespace detail {

class CommonDefinitions {
public:
  enum Level : unsigned int {
    PRAGMA,
    USER_DEFINITION,
    GENERATED_DEFINITION,
    SIZE
  };

  static CommonDefinitions& instance();

  static void append(const std::string& source, Level level);

  static Source getSource();

  CommonDefinitions(const CommonDefinitions&) = delete;
  CommonDefinitions& operator=(const CommonDefinitions&) = delete;

private:
  CommonDefinitions();

  std::vector<Source> _sources;
};

class RegisterCommonDefinition {
public:
  RegisterCommonDefinition(const char* definition,
                           CommonDefinitions::Level level
                              = CommonDefinitions::Level::USER_DEFINITION);
};

class RegisterCommonMacroDefinition {
public:
  template <typename T>
  RegisterCommonMacroDefinition(const char* name,
                                T&& value)
  {
    std::stringstream ss;
    ss << "#define " << name << " " << value;
    CommonDefinitions::append(ss.str(),
                              CommonDefinitions::Level::USER_DEFINITION);
  }
};

} // namespace detail

} // namespace skelcl

#endif // SOURCE_H_
