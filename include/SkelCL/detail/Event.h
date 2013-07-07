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
/// \file Event.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef EVENT_H_
#define EVENT_H_

#include <initializer_list>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

namespace skelcl {

namespace detail {

class Event {
public:
  Event();

  Event(const std::vector<cl::Event>& events);

  Event(std::initializer_list<cl::Event> events);

  //Event(const Event& rhs) = default;

  Event(Event&& rhs);

  //Event& operator=(const Event& rhs) = default;

  Event& operator=(Event&& rhs);

  //~Event() = default;

  void insert(const cl::Event& event);

  void wait();
private:
  std::vector<cl::Event> _events;
};

} // namespace detail

} // namespace skelcl

#endif // EVENT_H_

