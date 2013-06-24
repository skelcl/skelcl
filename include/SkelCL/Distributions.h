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
#include "detail/OLDistribution.h"
#include "detail/StencilDistribution.h"
#include "detail/CopyDistribution.h"
#include "detail/SingleDistribution.h"

namespace skelcl {

namespace detail { class Device; }

/// 
/// \brief  This namespace groups factory functions to construct the different
///         distributions available in SkelCL
/// 
namespace distribution {

// to prevent template argument deduction (i.e. template arguments have to be
// specified manually)
template <typename T>
struct identity {
  typedef T type;
};

/// 
/// \brief  Factory function to create a BlockDistribution with the types of the
///         given container.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           created. The complete type is C<T>.
/// \tparam T Type of the elements of the container for which the distribution
///           is created.
///
/// \param c  Container for which the distribution is created. This argument is
///           used to deduct the types needed to create the distribution which
///           gets returned.
///
/// \return A pointer to a newly created BlockDistribution with the types of the
///         given container.
/// 
template <template <typename> class C, typename T>
std::unique_ptr<skelcl::detail::Distribution<C<T>>>
    Block( const C<T>& /*c*/ )
{
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
            new skelcl::detail::BlockDistribution<C<T>>() );
}

/// 
/// \brief  This function sets the distribution of the given container to the
///         BlockDistribution.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           set. The complete type is C<T>.
/// \tparam T Type of the elements of the container for which the distribution
///           is set.
///
/// \param c  Container for which the distribution is set to BlockDistribution
///           using the setDistribution function.
/// 
template <template <typename> class C, typename T>
void setBlock( const C<T>& c)
{
  c.setDistribution( std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::BlockDistribution<C<T>>() ) );
}


template <template <typename> class C, typename T>
std::unique_ptr<skelcl::detail::Distribution<C<T>>>
    OL( const C<T>& /*c*/ )
{
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
            new skelcl::detail::OLDistribution<C<T>>() );
}

template <template <typename> class C, typename T>
void setOL( const C<T>& c)
{
  c.setDistribution( std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::OLDistribution<C<T>>() ) );
}
/// 
/// \brief  Factory function to create a CopyDistribution with the types of the
///         given container.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           created. The complete type is C<T>.
/// \tparam T Type of the elements of the container for which the distribution
///           is created.
///
/// \param c  Container for which the distribution is created. This argument is
///           used to deduct the types needed to create the distribution which
///           gets returned.
///
/// \return A pointer to a newly created CopyDistribution with the types of the
///         given container.
/// 
template <template <typename> class C, typename T>
std::unique_ptr<skelcl::detail::Distribution<C<T>>>
    Copy( const C<T>& /*c*/,
          typename identity<std::function<T(const T&, const T&)>>::type
            combineFunc = nullptr )
{
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::CopyDistribution<C<T>>(
                                    detail::globalDeviceList, combineFunc ) );
}

/// 
/// \brief  This function sets the distribution of the given container to the
///         CopyDistribution.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           set. The complete type is C<T>.
/// \tparam T Type of the elements of the container for which the distribution
///           is set.
///
/// \param c  Container for which the distribution is set to CopyDistribution
///           using the setDistribution function.
/// 
template <template <typename> class C, typename T>
void setCopy( const C<T>& c,
              std::function<T(const T&, const T&)> combineFunc = nullptr)
{
  c.setDistribution( std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::CopyDistribution<C<T>>(
          detail::globalDeviceList, combineFunc ) ) );
}

/// 
/// \brief  Factory function to create a SingleDistribution with the types of
///         the given container and for the default device.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           created. The complete type is C<T>.
/// \tparam T Type of the elements of the container for which the distribution
///           is created.
///
/// \param c  Container for which the distribution is created. This argument is
///           used to deduct the types needed to create the distribution which
///           gets returned.
///
/// \return A pointer to a newly created SingleDistribution with the types of
///         the given container and the default device.
/// 
template <template <typename> class C, typename T>
std::unique_ptr<skelcl::detail::Distribution<C<T>>>
    Single( const C<T>& /*c*/ )
{
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::SingleDistribution<C<T>>() );
}

/// 
/// \brief  Factory function to create a SingleDistribution with the types of
///         the given container and for the given device.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           created. The complete type is C<T>.
/// \tparam T Type of the elements of the container for which the distribution
///           is created.
///
/// \param c      Container for which the distribution is created. This argument
///               is used to deduct the types needed to create the distribution
///               which gets returned.
/// \param device The device used for the SingleDistribution.
///
/// \return A pointer to a newly created SingleDistribution with the types of
///         the given container and for the given device.
/// 
template <template <typename> class C, typename T>
std::unique_ptr<skelcl::detail::Distribution<C<T>>>
    Single( const C<T>& /*c*/,
            const std::shared_ptr<skelcl::detail::Device>& device )
{
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::SingleDistribution<C<T>>(device) );
}

/// 
/// \brief  This function sets the distribution of the given container to the
///         SingleDistribution using the default device.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           set. The complete type is C<T>.
/// \tparam T Type of the elements of the container for which the distribution
///           is set.
///
/// \param c  Container for which the distribution is set to SingleDistribution
///           using the default device.
/// 
template <template <typename> class C, typename T>
void setSingle( const C<T>& c )
{
  c.setDistribution( std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::SingleDistribution<C<T>>() ) );
}

/// 
/// \brief  This function sets the distribution of the given container to the
///         SingleDistribution using the given device.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           set. The complete type is C<T>.
/// \tparam T Type of the elements of the container for which the distribution
///           is set.
///
/// \param c      Container for which the distribution is set to
///               SingleDistribution using the given device.
/// \param device The device used for the SingleDistribution.
/// 
template <template <typename> class C, typename T>
void setSingle( const C<T>& c,
                const std::shared_ptr<skelcl::detail::Device>& device )
{
  c.setDistribution( std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::SingleDistribution<C<T>>(device) ) );
}

} // namespace distribution

namespace detail {

// provide function to clone arbitrary distribution while changing the
// template argument

/// 
/// \brief  This function provides the ability to clone arbitrary distributions
///         while changing the template arguments of the distributions.
///
/// \tparam T The type of the elements of the container to be converted in.
/// \tparam U The type of the elements of the container to be converted from.
/// \tparam C The incomplete type of the container. The complete types are C<U>
///           and C<T>
///
/// \param dist The distribution to be converted into the same distribution
///             replacing the template argument U by T.
///
/// \return A pointer to a newly created identical distribution as the given
///         argument where the template argument U is replaced by T.
/// 
template <typename T, typename U, template <typename> class C>
std::unique_ptr<Distribution<C<T>>>
    cloneAndConvert(const Distribution<C<U>>& dist)
{
  // block distribution
  auto block = dynamic_cast<const BlockDistribution<C<U>>*>(&dist);
  if (block != nullptr) {
    return std::unique_ptr<Distribution<C<T>>>(
            new BlockDistribution<C<T>>(*block) );
  }

  // copy distribution
  auto copy = dynamic_cast<const CopyDistribution<C<U>>*>(&dist);
  if (copy != nullptr) {
    return std::unique_ptr<Distribution<C<T>>>(
            new CopyDistribution<C<T>>(*copy) );
  }

  // single distribution
  auto single = dynamic_cast<const SingleDistribution<C<U>>*>(&dist);
  if (single != nullptr) {
    return std::unique_ptr<Distribution<C<T>>>(
            new SingleDistribution<C<T>>(*single) );
  }

  // overlap distribution
  auto ol = dynamic_cast<const OLDistribution<C<U>>*>(&dist);
  if (ol != nullptr) {
    return std::unique_ptr<Distribution<C<T>>>(
            new OLDistribution<C<T>>(*ol) );
  }

  // stencil distribution
  auto stencil = dynamic_cast<const StencilDistribution<C<U>>*>(&dist);
  if (stencil != nullptr) {
    return std::unique_ptr<Distribution<C<T>>>(
            new StencilDistribution<C<T>>(*stencil) );
  }

  // default distribution
  return std::unique_ptr<Distribution<C<T>>>(
            new Distribution<C<T>>(dist) );
}

} // namespace detail

} // namespace skelcl

#endif // DISTRIBUTIONS_H_

