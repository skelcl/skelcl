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
///  IndexVectorDef.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef INDEX_VECTOR_DEF_H_
#define INDEX_VECTOR_DEF_H_

#include <algorithm>
#include <ios>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>

#include "../Distributions.h"

#include "Device.h"
#include "DeviceBuffer.h"
#include "DeviceList.h"
#include "Distribution.h"
#include "Event.h"

namespace skelcl {

template <typename U>
void Vector<Index>::setDistribution(const detail::Distribution<Vector<U>>&
                                        origDistribution) const
{
  ASSERT(origDistribution.isValid());
  // convert and set distribution
  this->setDistribution(detail::cloneAndConvert<Index>(origDistribution));
}

template <typename U>
void
  Vector<Index>::setDistribution(
      const std::unique_ptr<detail::Distribution<Vector<U>>>&
          origDistribution) const
{
  ASSERT(origDistribution != nullptr);
  ASSERT(origDistribution->isValid());
  // convert and set distribution
  this->setDistribution(detail::cloneAndConvert<Index>(*origDistribution));
}

} // namespace skelcl

#endif // INDEX_VECTOR_DEF_H_

