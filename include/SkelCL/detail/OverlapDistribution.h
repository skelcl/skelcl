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
/// \file OverlapDistribution.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
/// \author Matthias Buss
///

#ifndef OVERLAP_DISTRIBUTION_H_
#define OVERLAP_DISTRIBUTION_H_

#include "Distribution.h"

#include "../Vector.h"
#include "../Matrix.h"

namespace skelcl {

namespace detail {

class DeviceList;

template <typename> class OverlapDistribution;

// Vector version
template <typename T>
class OverlapDistribution< Vector<T> > : public Distribution< Vector<T> > {
public:
  OverlapDistribution( Vector<T>::size_type overlapRadius = 1,
                       Padding padding = Padding::NEUTRAL,
                       T neutralElement = T(),
                       const DeviceList& deviceList = globalDeviceList );

  template <typename U>
  OverlapDistribution( const OverlapDistribution< Vector<U> >&rhs );

  ~OverlapDistribution();

  bool isValid() const;

  void startUpload(Vector<T>& container, Event* events) const;

  void startDownload(Vector<T>& container, Event* events) const;

  size_t sizeForDevice(const Vector<T>& container,
                       const detail::Device::id_type id) const;

  bool dataExchangeOnDistributionChange(Distribution< Vector<T> >& newDistribution);

  typename Vector<T>::size_type& overlapRadius() const;

  Padding& padding() const;

  T& neutralElement() const;

private:
  bool doCompare(const Distribution< Vector<T> >& rhs) const;

  Vector<T>::size_type            _overlapRadius;
  Padding                         _padding;
  T                               _neutralElement;
};

// Matrix version
template <typename T>
class OverlapDistribution< Matrix<T> > : public Distribution< Matrix<T> > {
public:
  OverlapDistribution( Matrix<T>::size_type::size_type overlapRadius = 1,
                       Padding padding = Padding::NEUTRAL,
                       T neutralElement = T(),
                       const DeviceList& deviceList = globalDeviceList );

  template <typename U>
  OverlapDistribution( const OverlapDistribution< Matrix<U> >&rhs );

  ~OverlapDistribution();

  bool isValid() const;

  void startUpload(Matrix<T>& container, Event* events) const;

  void startDownload(Matrix<T>& container, Event* events) const;

  size_t sizeForDevice(const Matrix<T>& container,
                       const detail::Device::id_type id) const;

  bool dataExchangeOnDistributionChange(Distribution< Matrix<T> >& newDistribution);

  typename Matrix<T>::size_type::size_type& overlapRadius() const;

  Padding& padding() const;

  T& neutralElement() const;

private:
  bool doCompare(const Distribution< Matrix<T> >& rhs) const;

  Matrix<T>::size_type::size_type _overlapRadius;
  Padding                         _padding;
  T                               _neutralElement;
};

} // namespace detail

} // namespace skelcl

#endif // BLOCK_DISTRIBUTION_H_

