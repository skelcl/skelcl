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

/// \cond
/// Don't show this forward declarations in doxygen
namespace detail { class Device; }
/// \endcond

/// 
/// \brief  This namespace groups factory functions to construct the different
///         distributions available in SkelCL
///
/// \ingroup distributions
/// 
namespace distribution {

// to prevent template argument deduction (i.e. template arguments have to be
// specified manually)
/// \cond
/// Don't show this helper class in the doxygen
template <typename T>
struct identity {
  typedef T type;
};
/// \endcond

/// 
/// \brief  Factory function to create a BlockDistribution with the types of the
///         given container.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           created. The complete type is C<T>. C can be Vector or Matrix.
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
    Block( const C<T>& c )
{
  (void)c;
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
            new skelcl::detail::BlockDistribution<C<T>>() );
}

/// 
/// \brief  This function sets the distribution of the given container to the
///         BlockDistribution.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           set. The complete type is C<T>. C can be Vector or Matrix.
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


/// \brief  Factory function to create an OverlapDistribution with the types of
///         the given container.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           created. The complete type is C<T>. C can be Vector or Matrix.
/// \tparam T Type of the elements of the container for which the distribution
///           is created.
///
/// \param c  Container for which the distribution is created. This argument is
///           used to deduct the types needed to create the distribution which
///           gets returned.
///
/// \return A pointer to a newly created OverlapDistribution with the types of
///         the given container.
/// 
template <template <typename> class C, typename T>
std::unique_ptr<skelcl::detail::Distribution<C<T>>>
    OL( const C<T>& c )
{
  (void)c;
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
            new skelcl::detail::OLDistribution<C<T>>() );
}

/// 
/// \brief  This function sets the distribution of the given container to the
///         OverlapDistribution.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           set. The complete type is C<T>. C can be Vector or Matrix.
/// \tparam T Type of the elements of the container for which the distribution
///           is set.
///
/// \param c  Container for which the distribution is set to OverlapDistribution
///           using the setDistribution function.
/// 
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
///           created. The complete type is C<T>. C can be Vector or Matrix.
/// \tparam T Type of the elements of the container for which the distribution
///           is created.
///
/// \param c  Container for which the distribution is created. This argument is
///           used to deduct the types needed to create the distribution which
///           gets returned.
///
/// \param combineFunc A binary function explaining how two elements of the
///                    container should be combined if two devices modify the
///                    data of a copy distributed container simultaneously.
///
/// \return A pointer to a newly created CopyDistribution with the types of the
///         given container.
/// 
template <template <typename> class C, typename T>
std::unique_ptr<skelcl::detail::Distribution<C<T>>>
    Copy( const C<T>& c,
          typename identity<std::function<T(const T&, const T&)>>::type
            combineFunc = nullptr )
{
  (void)c;
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::CopyDistribution<C<T>>(
                                    detail::globalDeviceList, combineFunc ) );
}

/// 
/// \brief  This function sets the distribution of the given container to the
///         CopyDistribution.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           set. The complete type is C<T>. C can be Vector or Matrix.
/// \tparam T Type of the elements of the container for which the distribution
///           is set.
///
/// \param c  Container for which the distribution is set to CopyDistribution
///           using the setDistribution function.
///
/// \param combineFunc A binary function explaining how two elements of the
///                    container should be combined if two devices modify the
///                    data of a copy distributed container simultaneously.
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
///           created. The complete type is C<T>. C can be Vector or Matrix.
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
    Single( const C<T>& c )
{
  (void)c;
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::SingleDistribution<C<T>>() );
}

/// 
/// \brief  Factory function to create a SingleDistribution with the types of
///         the given container and for the given device.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           created. The complete type is C<T>. C can be Vector or Matrix.
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
    Single( const C<T>& c,
            const std::shared_ptr<skelcl::detail::Device>& device )
{
  (void)c;
  return std::unique_ptr<skelcl::detail::Distribution<C<T>>>(
        new skelcl::detail::SingleDistribution<C<T>>(device) );
}

/// 
/// \brief  This function sets the distribution of the given container to the
///         SingleDistribution using the default device.
///
/// \tparam C Incomplete type of the container for which the distribution is
///           set. The complete type is C<T>. C can be Vector or Matrix.
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
///           set. The complete type is C<T>. C can be Vector or Matrix.
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


/// \cond
/// Don't show this internal namespace in doxygen
namespace detail {

// provide function to clone arbitrary distribution while changing the
// template argument

#if 0
// This solution does not work in visual studio, but enforces, that OutT and
// InT are of the same container type ...
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
#else
// This solution works in visual studio, but does not enforce, that OutT and
// InT are of the same container type ...
template <typename OutT, typename InT>
std::unique_ptr<Distribution<OutT>>
  cloneAndConvert(const Distribution<InT>& dist)
{
    // block distribution
    auto block = dynamic_cast<const BlockDistribution<InT>*>(&dist);
    if (block != nullptr) {
      return std::unique_ptr<Distribution<OutT>>(
        new BlockDistribution<OutT>(*block));
    }

    // copy distribution
    auto copy = dynamic_cast<const CopyDistribution<InT>*>(&dist);
    if (copy != nullptr) {
      return std::unique_ptr<Distribution<OutT>>(
        new CopyDistribution<OutT>(*copy));
    }

    // single distribution
    auto single = dynamic_cast<const SingleDistribution<InT>*>(&dist);
    if (single != nullptr) {
      return std::unique_ptr<Distribution<OutT>>(
        new SingleDistribution<OutT>(*single));
    }

    // overlap distribution
    auto ol = dynamic_cast<const OLDistribution<InT>*>(&dist);
    if (ol != nullptr) {
      return std::unique_ptr<Distribution<OutT>>(
        new OLDistribution<OutT>(*ol));
    }

    // stencil distribution
    auto stencil = dynamic_cast<const StencilDistribution<InT>*>(&dist);
    if (stencil != nullptr) {
      return std::unique_ptr<Distribution<OutT>>(
        new StencilDistribution<OutT>(*stencil) );
    }

    // default distribution
    return std::unique_ptr<Distribution<OutT>>(
      new Distribution<OutT>(dist));
}
#endif

} // namespace detail
/// \endcond

} // namespace skelcl

#endif // DISTRIBUTIONS_H_

