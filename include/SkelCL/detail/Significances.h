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
/// \file Significances.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef SIGNIFICANCES_H_
#define SIGNIFICANCES_H_

#include <initializer_list>
#include <vector>

namespace skelcl {

namespace detail {

class Device;

class Significances {
public:
  typedef float value_type;

  Significances() = default;
  Significances(size_t deviceCount);
  Significances(std::initializer_list<value_type> significances);
  Significances(const Significances&) = default;
  ~Significances() = default;
  Significances& operator=(const Significances&) = default;

  bool operator==(const Significances& rhs) const;

  void setSignificances(std::initializer_list<value_type> significances);

  value_type getSignificance(const Device::id_type device) const;

  size_t size() const;
private:
  std::vector<value_type> _values;
};

} // namespace detail

} // namespace skelcl

#endif // SIGNIFICANCES_H_
