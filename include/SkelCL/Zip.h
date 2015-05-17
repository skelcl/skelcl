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

#include "detail/Skeleton.h"
#include "detail/Program.h"

namespace skelcl {

/// \cond
/// Don't show this forward declarations in doxygen
class Source;
template <typename> class Out;

template<typename> class Zip;
/// \endcond

///
/// \defgroup zip Zip Skeleton
///
/// \brief The Zip skeleton describes a calculation on two containers performed
///        in parallel on one or more devices. It invokes a binary user-defined
///        function on its two input containers in a parallel fashion.
///
/// \ingroup skeletons
///

///
/// \brief An instance of the Zip class describes a calculation which can
///        be performed on one or more devices.
///
/// This is the most general version of the Zip skeleton.
/// It can be customized with a user-defined function taking arbitrary types as
/// arguments (no classes are currently possible) and produces an arbitrary
/// typed output.
///
/// On creation the Zip skeleton is customized with source code defining
/// a binary function.
/// The Zip skeleton can be executed by passing two containers. The given
/// function is executed ones for every pair of item of the two input
/// containers.
///
/// More formally: When x and y are two containers of length n and m with items
/// x[0] .. x[n-1] and y[0] .. y[m-1], where m is equal or greater than n,
/// f is the provided function, the Zip skeleton calculates the output container
/// z as follows: z[i] = f(x[i], y[i]) for every i in 0 .. n-1.
///
/// \tparam Tleft  Type of the left input data of the skeleton
/// \tparam Tright Type of the right input data of the skeleton
/// \tparam Tout   Type of the output data of the skeleton
///
/// \ingroup skeletons
/// \ingroup zip
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
  /// \param funcName Name of the 'main' function (the starting point)
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
  /// \param right The second (right) input data for the skeleton managed inside
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
  /// \param args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <template <typename> class C,
            typename... Args>
  C<Tout> operator()(const C<Tleft>& left,
                     const C<Tright>& right,
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
  /// \param left   The first (left) input data for the skeleton managed inside
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
  /// \param right  The second (right) input data for the skeleton managed
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
  /// \param args   Additional arguments which are passed to the function
  ///               named by funcName and defined in the source code at created.
  ///               The individual arguments must be passed in the same order
  ///               here as they where defined in the funcName function
  ///               declaration.
  ///
  template <template <typename> class C,
            typename... Args>
  C<Tout>& operator()(Out<C<Tout>> output,
                      const C<Tleft>& left,
                      const C<Tright>& right,
                      Args&&... args);

  ///
  /// \brief Return the source code of the user defined function.
  ///
  /// \return The source code of the user defined function.
  ///
  const std::string& source() const;
 
  ///
  /// \brief Return the name of the user defined function.
  ///
  /// \return The name of the user defined function.
  ///
  const std::string& func() const;

private:
  template <template <typename> class C,
            typename... Args>
  void execute(C<Tout>& output,
               const C<Tleft>& left,
               const C<Tright>& right,
               Args&&... args);

  template <template <typename> class C>
  void prepareInput(const C<Tleft>& left,
                    const C<Tright>& right);

  template <template <typename> class C>
  void prepareOutput(C<Tout>& output,
                     const C<Tleft>& left,
                     const C<Tright>& right);
  
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;

  const std::string     _source;
  const std::string     _funcName;
  const detail::Program _program;
};

///
/// \brief An instance of the Zip class describes a calculation which can
///        be performed on one or more devices.
///
/// This is a more specialized version of the general Zip<Tout(Tleft, Tright)>
/// skeleton.
/// It can be customized with a user-defined function taking arbitrary types as
/// arguments (no classes are currently possible) and produces no output.
///
/// On creation the Zip skeleton is customized with source code defining
/// a binary function.
/// The Zip skeleton can be executed by passing two containers. The given
/// function is executed ones for every pair of item of the two input
/// containers.
///
/// More formally: When x and y are two containers of length n and m with items
/// x[0] .. x[n-1] and y[0] .. y[m-1], where m is equal or greater than n,
/// f is the provided function, the Zip skeleton calculates the output container
/// z as follows: z[i] = f(x[i], y[i]) for every i in 0 .. n-1.
///
/// \tparam Tleft  Type of the left input data of the skeleton
/// \tparam Tright Type of the right input data of the skeleton
/// \tparam Tout   Type of the output data of the skeleton
///
/// \ingroup skeletons
/// \ingroup zip
///
template<typename Tleft,
         typename Tright>
class Zip<void(Tleft, Tright)> : public detail::Skeleton {
public:
  ///
  /// \brief Constructor taking the source code used to customize the Zip
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  /// \param funcName Name of the 'main' function (the starting point)
  ///                 of the given source code
  ///
  Zip<void(Tleft, Tright)>(const Source& source,
                           const std::string& funcName
                                              = std::string("func"));

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments left, right and args.
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
  /// \param right The second (right) input data for the skeleton managed inside
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
  /// \param args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <template <typename> class C,
            typename... Args>
  void operator()(const C<Tleft>& left,
                  const C<Tright>& right,
                  Args&&... args);

private:
  template <template <typename> class C,
            typename... Args>
  void execute(const C<Tleft>& left,
               const C<Tright>& right,
               Args&&... args);

  template <template <typename> class C>
  void prepareInput(const C<Tleft>& left,
                    const C<Tright>& right);

  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;

  const detail::Program _program;
};

// TODO: when template aliases are available:
// template<typename T>
// using Zip = Zip<T(T, T)>;

} // namespace skelcl

#include "detail/ZipDef.h"

#endif // ZIP_H_

