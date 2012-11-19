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
/// \file SingleDistribution.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef SINGLE_DISTRIBUTION_H_
#define SINGLE_DISTRIBUTION_H_

#include "Distribution.h"

namespace skelcl {

template <typename> class Matrix;
template <typename> class Vector;

namespace detail {

template <typename> class SingleDistribution;

template <template <typename> class C, typename T>
class SingleDistribution< C<T> > : public Distribution< C<T> > {
public:
  SingleDistribution(std::shared_ptr<Device> device = detail::globalDeviceList.front());

  template <typename U>
  SingleDistribution( const SingleDistribution< C<U> >& rhs);

  ~SingleDistribution();

  bool isValid() const;

  void startUpload(C<T>& container,
                   Event* events) const;

  void startDownload(C<T>& container,
                     Event* events) const;

  size_t sizeForDevice(const C<T>& container,
                       const std::shared_ptr<detail::Device>& devicePtr) const;

  bool dataExchangeOnDistributionChange(Distribution< C<T> >& newDistribution);

private:
  bool doCompare(const Distribution< C<T> >& rhs) const;
};

namespace single_distribution_helper {

template <typename T>
size_t sizeForDevice(const typename Vector<T>::size_type size);

template <typename T>
size_t sizeForDevice(const typename Matrix<T>::size_type size);

} // namespace single_distribution_helper

} // namespace detail

} // namespace skelcl

#include "SingleDistributionDef.h"

#endif // SINGLE_DISTRIBUTION_H_

