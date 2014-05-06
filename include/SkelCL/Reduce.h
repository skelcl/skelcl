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

template <typename> class Out;
template <typename> class Vector;
namespace detail { class DeviceList; }

template <typename> class Reduce;

///
/// \class Reduce
///
/// \brief An instance of the Reduce skeleton describes a reduction calculation
///        (a.k.a. accumulate) customized by a given source code.
///
/// On creating the Reduce skeleton is customized with source code defining
/// a (at least) binary function.
/// The Reduce skeleton can than be called by passing an input container.
/// The given function is applied repetitively to calculate a single scalar
/// value from all items of the input container.
/// More formally: When x is a container of length n with items x[0] .. x[n-1],
/// f is the provided function, the Reduce skeleton calculates the output value
/// y as follows: y = f(..f(f(x[0], x[1]), x[2]),.. x[n-1]) or written more
/// common using + as a symbol for the given function and using infix notation:
/// y = x[0] + x[1] + x[2] ... + x[n-1].
///
template<typename T>
class Reduce<T(T)> : public detail::Skeleton {
public:
  ///
  /// \brief Constructor taking the source code to customize the Reduce
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  ///        id       Identity for he function named by funcName and defined in
  ///                 source. Meaning: if func is the name of the function
  ///                 defined in source, func(x, id) = x and func(id, x) = x
  ///                 for every possible value of x
  ///
  ///        funcName Name of the 'main' function (the starting point) of the
  ///                 given source code
  ///
  Reduce(const Source& source,
         const std::string& id = "0",
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
  ///              input vector! This is certainly advanced later.
  ///
  ///        args  Additional arguments which are passed to the function
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
  ///        input  The input data for the skeleton managed inside a Vector.
  ///               If no distribution is set the Single distribution using the
  ///               device with id 0 is used.
  ///               Currently only the Single distribution is supported for the
  ///               input vector! This is certainly advanced later.
  ///
  ///        args   Additional arguments which are passed to the function
  ///               named by funcName and defined in the source code at
  ///               created. The individual arguments must be passed in the 
  ///               same order here as they where defined in the funcName 
  ///               function declaration.
  ///
  template <typename... Args>
  Vector<T>& operator()(Out<Vector<T>> output,
                        const Vector<T>& input,
                        Args&&... args);

  const std::string source() const {
      return _userSource;
  }

  const std::string func() const {
      return _funcName;
  }

  const std::string id() const {
      return _id;
  }

private:

  ///
  /// \brief Prepares the input for kernel execution
  ///
  /// \param input The vector to be prepared
  ///
  void prepareInput(const Vector<T>& input);

  ///
  /// \brief Prepares the output for kernel execution
  ///
  /// \param output The vector to be prepared
  ///        input  The input vector is used to determine the distribution to
  ///               select for the output vector
  ///        size   After the call, the output vector is guaranteed to be able
  ///               to store at least size many elements
  ///
  void prepareOutput(Vector<T>& output,
                     const Vector<T>& input,
                     const size_t size);

  ///
  /// \brief Queries the execution of one level of the reduce algorithm for a
  ///        given device
  ///
  /// \param device The device on which the execution is to be queried
  ///        level  The level to be executed of the reduce algorithm
  ///        args   Additional arguments
  ///
  template <typename... Args>
  void execute_first_step(const detail::Device& device,
                          const detail::DeviceBuffer& input,
                                detail::DeviceBuffer& output,
                                size_t data_size,
                                size_t global_size,
                                Args&&... args);

  template <typename... Args>
  void execute_second_step(const detail::Device& device,
                           const detail::DeviceBuffer& input,
                                 detail::DeviceBuffer& output,
                                 size_t data_size,
                                 Args&&... args);


  std::shared_ptr<skelcl::detail::Program>
    createPrepareAndBuildProgram();

  /// Literal describing the identity of type T in respect to the operation
  /// performed by the reduction and described in function named _funcName
  std::string _id;

  /// Name of the main function defined in _userSource
  std::string _funcName;

  /// Source code as defined by the application developer
  std::string _userSource;

  /// Program
  std::shared_ptr<skelcl::detail::Program> _program;


};

} // namespace skelcl

#include "detail/ReduceDef.h"

#endif // REDUCE_H_

