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
/// \file Distributions.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef DISTRIBUTIONS_H_
#define DISTRIBUTIONS_H_

// include all distributions

#include "detail/Distribution.h"
#include "detail/BlockDistribution.h"
#include "detail/CopyDistribution.h"
#include "detail/SingleDistribution.h"

namespace skelcl {

namespace distribution {

template <template <typename> class C, typename T>
std::unique_ptr< skelcl::detail::Distribution< C<T> > >
    Block( const C<T>& /*c*/ )
{
  return std::unique_ptr< skelcl::detail::Distribution< C<T> > >(
            new skelcl::detail::BlockDistribution< C<T> >() );
}

template <template <typename> class C, typename T>
std::unique_ptr< skelcl::detail::Distribution< C<T> > >
    Copy( const C<T>& /*c*/ )
{
  return std::unique_ptr< skelcl::detail::Distribution< C<T> > >(
            new skelcl::detail::CopyDistribution< C<T> >() );
}

template <template <typename> class C, typename T>
std::unique_ptr< skelcl::detail::Distribution< C<T> > >
    Single( const C<T>& /*c*/ )
{
  return std::unique_ptr< skelcl::detail::Distribution< C<T> > >(
            new skelcl::detail::SingleDistribution< C<T> >() );
}

template <template <typename> class C, typename T>
std::unique_ptr< skelcl::detail::Distribution< C<T> > >
    Default( const C<T>& /*c*/ )
{
  return std::unique_ptr< skelcl::detail::Distribution< C<T> > >(
            new skelcl::detail::Distribution< C<T> >() );
}

} // namespace distribution

namespace detail {

// provide function to clone arbitrary distribution while changing the
// template argument

template <typename T, typename U, template <typename> class C>
std::unique_ptr< Distribution< C<T> > >
    cloneAndConvert(const Distribution< C<U> >& dist)
{
  // block distribution
  auto block = dynamic_cast<const BlockDistribution< C<U> >*>(&dist);
  if (block != nullptr) {
    return std::unique_ptr< Distribution< C<T> > >(
            new BlockDistribution< C<T> >(*block) );
  }

  // copy distribution
  auto copy = dynamic_cast<const CopyDistribution< C<U> >*>(&dist);
  if (copy != nullptr) {
    return std::unique_ptr< Distribution< C<T> > >(
            new CopyDistribution< C<T> >(*copy) );
  }

  // single distribution
  auto single = dynamic_cast<const SingleDistribution< C<U> >*>(&dist);
  if (single != nullptr) {
    return std::unique_ptr< Distribution< C<T> > >(
            new SingleDistribution< C<T> >(*single) );
  }

  // default distribution
  return std::unique_ptr< Distribution< C<T> > >(
            new Distribution< C<T> >(dist) );
}

} // namespace detail

} // namespace skelcl

#endif // DISTRIBUTIONS_H_

