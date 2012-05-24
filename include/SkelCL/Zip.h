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
/// \file Zip.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef ZIP_H_
#define ZIP_H_

#include <istream>
#include <string>

#include "Source.h"

#include "detail/Program.h"
#include "detail/Skeleton.h"

namespace skelcl {

template <typename> class Out;
namespace detail { template <typename> class Container; }

template<typename> class Zip;

///
/// \class Zip
///
/// \brief An instance of the Zip skeleton describes a calculation which can
///        be performed on a device.
///
/// \tparam Tleft  Type of the left input data of the skeleton
///         Tright Type of the right input data of the skeleton
///         Tout   Type of the output data of the skeleton
///
/// On creation the Zip skeleton is customized with source code defining
/// a (at least) binary function.
/// The Zip skeleton can than be called by passing two containers of the same
/// type. The given function is executed ones for every pair of item of the two
/// input containers.
/// More formally: When x and y are two containers of length n and m with items
/// x[0] .. x[n-1] and y[0] .. y[m-1], where m is equal or greater than n,
/// f is the provided function, the Zip skeleton calculates the output container
/// z as follows: z[i] = f(x[i], y[i]) for every i in 0 .. n-1.
///
template<typename Tleft,
         typename Tright,
         typename Tout>
class Zip<Tout(Tleft, Tright)> : public detail::Skeleton {
public:
  ///
  /// \brief Constructor taking the source code used to customize the Zip
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  ///        funcName Name of the 'main' function (the starting point)
  ///                 of the given source code
  ///
  Zip<Tout(Tleft, Tright)>(const Source& source,
                           const std::string& funcName
                                              = std::string("func"));

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments left, right and args. The resulting data is returned
  ///        as a moved copy.
  ///
  /// \param left  The first (left) input data for the skeleton managed inside
  ///              a container. The actual Type of this argument must be a
  ///              subtype of the Container class.
  ///              If no distribution is set for this container as well as for
  ///              the right container as default the Block distribution is
  ///              selected.
  ///              If no distribution is set for this container and a
  ///              distribution is set for the right container the distribution
  ///              from the right container is adopted.
  ///              If a distribution is set and differs from the distribution of
  ///              the right distribution the Block distribution is set for both
  ///              containers.
  ///
  ///        right The second (right) input data for the skeleton managed inside
  ///              a container. The actual Type of this argument must be a
  ///              subtype of the Container class.
  ///              If no distribution is set for this container as well as for
  ///              the left container as default the Block distribution is
  ///              selected.
  ///              If no distribution is set for this container and a
  ///              distribution is set for the left container the distribution
  ///              from the left container is adopted.
  ///              If a distribution is set and differs from the distribution of
  ///              the left distribution the Block distribution is set for both
  ///              containers.
  ///
  ///        args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <template <typename> class ContainerType,
            typename... Args>
  ContainerType<Tout> operator()(const ContainerType<Tleft>& left,
                                 const ContainerType<Tright>& right,
                                 Args&&... args);

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments left, right  and args. The resulting data is stored
  ///        in the provided container output. A reference to the output
  ///        container is returned to allow for chaining skeleton calls.
  ///
  /// \param output The container storing the result of the execution of the
  ///               skeleton. A reference to this container is also returned.
  ///               The container might be resized to fit the result.
  ///               The distribution of the container might change.
  ///
  ///        left   The first (left) input data for the skeleton managed inside
  ///               a container. The actual Type of this argument must be a
  ///               subtype of the Container class.
  ///               If no distribution is set for this container as well as for
  ///               the right container as default the Block distribution is
  ///               selected.
  ///               If no distribution is set for this container and a
  ///               distribution is set for the right container the distribution
  ///               from the right container is adopted.
  ///               If a distribution is set and differs from the distribution
  ///               of the right distribution the Block distribution is set for
  ///               both containers.
  ///
  ///        right  The second (right) input data for the skeleton managed
  ///               inside a container. The actual Type of this argument must be
  ///               a subtype of the Container class.
  ///               If no distribution is set for this container as well as for
  ///               the left container as default the Block distribution is
  ///               selected.
  ///               If no distribution is set for this container and a
  ///               distribution is set for the left container the distribution
  ///               from the left container is adopted.
  ///               If a distribution is set and differs from the distribution
  ///               of the left distribution the Block distribution is set for
  ///               both containers.
  ///
  ///        args   Additional arguments which are passed to the function
  ///               named by funcName and defined in the source code at created.
  ///               The individual arguments must be passed in the same order
  ///               here as they where defined in the funcName function
  ///               declaration.
  ///
  template <template <typename> class ContainerType,
            typename... Args>
  ContainerType<Tout>& operator()(Out<ContainerType<Tout>> output,
                                  const ContainerType<Tleft>& left,
                                  const ContainerType<Tright>& right,
                                  Args&&... args);

private:
  ///
  /// \brief Create a program object from the provided source string
  ///
  /// \param source The source code defined by the application developer
  ///
  /// \return A program object customized with the source code defined by
  ///         the application developer, as well as the zip skeleton's
  ///         kernel implementation
  ///
  detail::Program createProgram(const std::string& source) const;

  ///
  /// \brief Modifies the program to be a valid OpenCL kernel and builds it
  ///
  /// \param funcName The name of the 'main' function, i.e. the entry point in
  ///                 the application developer's source code
  ///
  void prepareAndBuildProgram(const std::string& funcName);

  ///
  /// \brief Prepares the inputs for kernel execution
  ///
  /// \param left  The first (left) input container to be prepared
  ///        right The second (right) input container to be prepared
  ///
  void prepareInput(const detail::Container<Tleft>& left,
                    const detail::Container<Tright>& right);

  ///
  /// \brief Prepares the output for kernel execution
  ///
  /// \param output The container to be prepared
  ///        left   The first (left) input vector is used to determine the 
  ///               distribution to select for the output vector as well as
  ///               it's size
  ///        right  The second (right) input vector is used to determine the
  ///               distribution to select for the output vector as well as
  ///               it's size
  ///
  void prepareOutput(detail::Container<Tout>& output,
                     const detail::Container<Tleft>& left,
                     const detail::Container<Tright>& right);

  ///
  /// \brief Queries the actual execution of the zip skeleton's kernel
  ///
  /// \param output The ouput container
  ///        left   The first (left) input container
  ///        right  The second (right) input container
  ///        args   Additional arguments
  ///
  template <typename... Args>
  void execute(detail::Container<Tout>& output,
               const detail::Container<Tleft>& left,
               const detail::Container<Tright>& right,
               Args&&... args);

  /// Program describing the operation performed by the zip skeleton
  detail::Program _program;
};

// TODO: when template aliases are available:
// template<typename T>
// using Zip = Zip<T(T, T)>;

} // namespace skelcl

#include "detail/ZipDef.h"

#endif // ZIP_H_

