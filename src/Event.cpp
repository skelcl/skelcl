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
/// \file Event.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include <pvsutil/Logger.h>

#include "SkelCL/detail/Event.h"

namespace skelcl {

namespace detail {


Event::Event()
  : _events()
{
}

Event::Event(const std::vector<cl::Event>& events)
  : _events(events)
{
}

Event::Event(std::initializer_list<cl::Event> events)
  : _events(events.begin(), events.end())
{
}

Event::Event(Event&& rhs)
  : _events(std::move(rhs._events))
{
}

Event& Event::operator=(Event&& rhs)
{
  _events = std::move(rhs._events);
  return *this;
}

void Event::insert(const cl::Event& event)
{
  _events.push_back(event);
}

void Event::wait()
{
  try {
    // Wait for every single device, because of different context objects
    std::for_each( _events.begin(), _events.end(),
                   std::mem_fn(&cl::Event::wait) );
  } catch (cl::Error& err) {
    ABORT_WITH_ERROR(err);
  }
}

} // namespace detail

} // namespace skelcl
