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
/// \file Reduce.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef REDUCE_H_
#define REDUCE_H_

#include <istream>
#include <memory>
#include <string>

#include "Source.h"

#include "detail/Program.h"
#include "detail/Skeleton.h"

namespace skelcl {

/// \cond
/// Don't show this forward declarations in doxygen
template <typename> class Out;
template <typename> class Vector;
namespace detail { class DeviceList; }

template <typename> class Reduce;
/// \endcond

///
/// \defgroup reduce Reduce Skeleton
///
/// \brief The Reduce skeleton describes a calculation on a Vector performed in
///        parallel on a device. Given a binary user-defined function the input
///        Vector is reduced to a scalar value.
///
/// \ingroup skeletons
///

///
/// \brief An instance of the Reduce class describes a reduction calculation
///        (a.k.a. accumulate) customized by a given binary user-defined
///        function.
///
/// On creation the Reduce skeleton is customized with source code defining
/// a binary function. The binary function has to be associative and
/// commutative. The Reduce skeleton can be executed by passing an input Vector.
/// The given function is applied repetitively to calculate a single scalar
/// value from all items of the input container.
///
/// More formally: When v is a Vector of length n with items v[0] .. v[n-1],
/// f is the provided function, the Reduce skeleton calculates the output value
/// y as follows: y = f(..f(f(v[0], v[1]), v[2]),.. v[n-1]) or written more
/// easily using + as a symbol for the given function and using infix notation:
/// y = v[0] + v[1] + v[2] ... + v[n-1].
///
/// \tparam T Type of the input and output data of the skeleton.
///
/// \ingroup skeletons
/// \ingroup reduce
///
template <typename T>
class Reduce<T(T)> : public detail::Skeleton {
public:
  ///
  /// \brief Constructor taking the source code to customize the Reduce
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton.
  ///
  /// \param id       Identity for he function named by funcName and defined in
  ///                 source. Meaning: if func is the name of the function
  ///                 defined in source, func(x, id) = x and func(id, x) = x
  ///                 for every possible value of x
  ///
  /// \param funcName Name of the 'main' function (the starting point) of the
  ///                 given source code
  ///
  Reduce(const Source& source, const std::string& id = "0",
         const std::string& funcName = "func");

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as argument input and args. The resulting data is returned as a
  ///        moved copy.
  ///
  /// \param input The input data for the skeleton managed inside a Vector.
  ///              If no distribution is set the Single distribution using the
  ///              device with id 0 is used.
  ///              Currently only the Single distribution is supported for the
  ///              input Vector! This will certainly be advanced in a future
  ///              version.
  ///
  /// \param args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <typename... Args>
  Vector<T> operator()(const Vector<T>& input, Args&&... args);

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as argument input and args. The resulting data is stored in the
  ///        provided Vector output. A reference to the output Vector is
  ///        returned to allow for chaining skeleton calls.
  ///
  /// \param output The Vector storing the result of the execution of the
  ///               skeleton. A reference to this container is also returned.
  ///               The Vector might be resized to fit the result.
  ///               The distribution of the container might change.
  ///
  /// \param input  The input data for the skeleton managed inside a Vector.
  ///               If no distribution is set the Single distribution using the
  ///               device with id 0 is used.
  ///               Currently only the Single distribution is supported for the
  ///               input vector! This will certainly be advanced in a future
  ///               version.
  ///
  /// \param args   Additional arguments which are passed to the function
  ///               named by funcName and defined in the source code at
  ///               created. The individual arguments must be passed in the 
  ///               same order here as they where defined in the funcName 
  ///               function declaration.
  ///
  template <typename... Args>
  Vector<T>& operator()(Out<Vector<T>> output, const Vector<T>& input,
                        Args&&... args);

  ///
  /// \brief Return the source code of the user defined function.
  ///
  /// \return The source code of the user defined function.
  ///
  std::string source() const;

  ///
  /// \brief Return the name of the user defined function.
  ///
  /// \return The name of the user defined function.
  ///
  std::string func() const;

  ///
  /// \brief Return the identity for the user defined function.
  ///
  /// \return The identity for the user defined function.
  ///
  std::string id() const;

private:
  void prepareInput(const Vector<T>& input);

  void prepareOutput(Vector<T>& output, const Vector<T>& input,
                     const size_t size);

  template <typename... Args>
  void execute_first_step(const detail::Device& device,
                          const detail::DeviceBuffer& input,
                          detail::DeviceBuffer& output, size_t data_size,
                          size_t global_size, Args&&... args);

  template <typename... Args>
  void execute_second_step(const detail::Device& device,
                           const detail::DeviceBuffer& input,
                           detail::DeviceBuffer& output, size_t data_size,
                           Args&&... args);

  skelcl::detail::Program createPrepareAndBuildProgram();

  /// Literal describing the identity of type T in respect to the operation
  /// performed by the reduction and described in function named _funcName
  std::string _id;

  /// Name of the main function defined in _userSource
  std::string _funcName;

  /// Source code as defined by the application developer
  std::string _userSource;

  /// Program
  skelcl::detail::Program _program;
};

} // namespace skelcl

#include "detail/ReduceDef.h"

#endif // REDUCE_H_

