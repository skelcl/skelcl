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
/// \file Map.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef MAP_H_
#define MAP_H_

#include <istream>
#include <string>

#include "detail/MapHelper.h"
#include "detail/Skeleton.h"

namespace skelcl {

class Index;
class IndexPoint;
class Source;
template <typename> class Out;
namespace detail { class Program; }

template<typename> class Map;

///
/// \class Map
///
/// \brief An instance of the Map skeleton describes a calculation which can
///        be performed on a device.
///
/// \tparam Tin  Type of the input data of the skeleton
///         Tout Type of the output data of the skeleton
///
/// On creation the Map skeleton is customized with source code defining
/// a (at least) unary function.
/// The Map skeleton can than be called by passing an input container.
/// The given function is executed ones for every item of the input container.
/// More formally: When x is a container of length n with items x[0] .. x[n-1],
/// f is the provided function, the Map skeleton calculates the output
/// container y as follows: y[i] = f(x[i]) for every i in 0 .. n-1.
///
/// This is the general version taking two template arguments describing the
/// type of the input data and the type of the output data as well.
/// A more specific version of the Map skeleton without output data is also
/// available.
///
template<typename Tin, typename Tout>
class Map<Tout(Tin)> : public detail::Skeleton,
                       private detail::MapHelper<Tout(Tin)> {
public:
  ///
  /// \brief Constructor taking the source code used to customize the Map
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  ///        funcName Name of the 'main' function (the staring point)
  ///                 of the given source code
  ///
  Map<Tout(Tin)>(const Source& source,
                 const std::string& funcName = std::string("func"));

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments input and args. The resulting data is returned as a
  ///        moved copy.
  ///
  /// \param input The input data for the skeleton managed inside a container.
  ///              The actual Type of this argument must be a subtype of the
  ///              Container class.
  ///              If no distribution is set for this container as default the
  ///              Block distribution is selected.
  ///
  ///        args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <template <typename> class C,
            typename... Args>
  C<Tout> operator()(const C<Tin>& input,
                     Args&&... args) const;

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments input and args. The resulting data is stored in the
  ///        provided container output. A reference to the output container is
  ///        returned to allow for chaining skeleton calls.
  ///
  /// \param output The container storing the result of the execution of the
  ///               skeleton. A reference to this container is also returned.
  ///               The container might be resized to fit the result.
  ///               The distribution of the container might change.
  ///
  ///        input  The input data for the skeleton managed inside a container.
  ///               The actual Type of this argument must be a subtype of the
  ///               Container class.
  ///               If no distribution is set for this container as default the
  ///               Block distribution is selected.
  ///
  ///        args   Additional arguments which are passed to the function
  ///               named by funcName and defined in the source code at created.
  ///               The individual arguments must be passed in the same order
  ///               here as they where defined in the funcName function
  ///               declaration.
  ///
  template <template <typename> class C,
            typename... Args>
  C<Tout>& operator()(Out< C<Tout> > output,
                      const C<Tin>&  input,
                      Args&&... args) const;

private:
  /// \brief Performs the actual execution of the map skeleton's kernel
  ///
  /// \param output The output container
  ///        input  The input container
  ///        args   Additional arguments
  ///
  template <template <typename> class C,
            typename... Args>
  void execute(C<Tout>& output,
               const C<Tin>& input,
               Args&&... args) const;

  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

///
/// \class Map
///
/// \brief An instance of the Map skeleton describes a calculation which can
///        be performed on a device.
///
/// \tparam Tin  Type of the input data of the skeleton
///
/// On creation the Map skeleton is customized with source code for a function.
/// The Map skeleton can than be called by passing a container.
/// The function is executed ones for every item of the container.
/// More formally: When x is a container of length n with items x[0] .. x[n-1],
/// f is the provided function, the Map skeleton executes as follows:
/// f(x[i]) for every i in 0 .. n-1.
///
/// This is a version taking only onea template arguments describing the type
/// of the input data. A more general version defining a Map skeleton which
/// can provide output data is also available.
///
template<typename Tin>
class Map<void(Tin)> : public detail::Skeleton,
                       private detail::MapHelper<void(Tin)> {
public:
  ///
  /// \brief Constructor taking the source code used to customize the Map
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  ///        funcName Name of the 'main' function (the staring point)
  ///                 of the given source code
  ///
  Map<void(Tin)>(const Source& source,
                 const std::string& funcName = std::string("func"));

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments input and args.
  ///
  /// \param input The input data for the skeleton managed inside a container.
  ///              The actual Type of this argument must be a subtype of the
  ///              Container class.
  ///              If no distribution is set for this container as default the
  ///              Block distribution is selected.
  ///
  ///        args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <template <typename> class C,
            typename... Args>
  void operator()(const C<Tin>& input,
                  Args&&... args) const;

private:
  /// \brief Queries the actual execution of the map skeleton's kernel
  ///
  /// \param input  The input container
  ///        args   Additional arguments
  ///
  template <template <typename> class C,
            typename... Args>
  void execute(const C<Tin>& input,
               Args&&... args) const;

  /// \brief Create a program object from the provided source string
  ///
  /// \param source The source code defined by the application developer
  ///
  /// \return A program object customized with the source code defined by
  ///         the application developer, as well as the map skeleton's
  ///         kernel implementation
  ///
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

///
/// \class Map
///
///
template<typename Tout>
class Map<Tout(Index)> : public detail::Skeleton,
                         private detail::MapHelper<Tout(Index)> {
public:
  ///
  /// \brief Constructor taking the source code used to customize the Map
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  ///        funcName Name of the 'main' function (the staring point)
  ///                 of the given source code
  ///
  Map<Tout(Index)>(const Source& source,
                   const std::string& funcName = std::string("func"));

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments input and args.
  ///
  /// \param input The input data for the skeleton managed inside a container.
  ///              The actual Type of this argument must be a subtype of the
  ///              Container class.
  ///              If no distribution is set for this container as default the
  ///              Block distribution is selected.
  ///
  ///        args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <typename... Args>
  Vector<Tout> operator()(const Vector<Index>& input,
                          Args&&... args) const;

  template <typename... Args>
  Vector<Tout>& operator()(Out< Vector<Tout> > output,
                           const Vector<Index>& input,
                           Args&&... args) const;

private:
  /// \brief Queries the actual execution of the map skeleton's kernel
  ///
  /// \param input  The input container
  ///        args   Additional arguments
  ///
  template <typename... Args>
  void execute(Vector<Tout>& output,
               const Vector<Index>& input,
               Args&&... args) const;

  /// \brief Create a program object from the provided source string
  ///
  /// \param source The source code defined by the application developer
  ///
  /// \return A program object customized with the source code defined by
  ///         the application developer, as well as the map skeleton's
  ///         kernel implementation
  ///
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

///
/// \class Map
///
///
template<>
class Map<void(Index)> : public detail::Skeleton,
                         private detail::MapHelper<void(Index)> {
public:
  ///
  /// \brief Constructor taking the source code used to customize the Map
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  ///        funcName Name of the 'main' function (the staring point)
  ///                 of the given source code
  ///
  Map<void(Index)>(const Source& source,
                   const std::string& funcName = std::string("func"));

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments input and args.
  ///
  /// \param input The input data for the skeleton managed inside a container.
  ///              The actual Type of this argument must be a subtype of the
  ///              Container class.
  ///              If no distribution is set for this container as default the
  ///              Block distribution is selected.
  ///
  ///        args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <typename... Args>
  void operator()(const Vector<Index>& input,
                  Args&&... args) const;

private:
  /// \brief Queries the actual execution of the map skeleton's kernel
  ///
  /// \param input  The input container
  ///        args   Additional arguments
  ///
  template <typename... Args>
  void execute(const Vector<Index>& input,
               Args&&... args) const;

  /// \brief Create a program object from the provided source string
  ///
  /// \param source The source code defined by the application developer
  ///
  /// \return A program object customized with the source code defined by
  ///         the application developer, as well as the map skeleton's
  ///         kernel implementation
  ///
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

///
/// \class Map
///
///
template<typename Tout>
class Map<Tout(IndexPoint)> : public detail::Skeleton,
                              private detail::MapHelper<Tout(IndexPoint)> {
public:
  ///
  /// \brief Constructor taking the source code used to customize the Map
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  ///        funcName Name of the 'main' function (the staring point)
  ///                 of the given source code
  ///
  Map<Tout(IndexPoint)>(const Source& source,
                        const std::string& funcName = std::string("func"));
  
  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments input and args.
  ///
  /// \param input The input data for the skeleton managed inside a container.
  ///              The actual Type of this argument must be a subtype of the
  ///              Container class.
  ///              If no distribution is set for this container as default the
  ///              Block distribution is selected.
  ///
  ///        args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <typename... Args>
  Matrix<Tout> operator()(const Matrix<IndexPoint>& input,
                          Args&&... args) const;
  
  template <typename... Args>
  Matrix<Tout>& operator()(Out< Matrix<Tout> > output,
                           const Matrix<IndexPoint>& input,
                           Args&&... args) const;
  
private:
  /// \brief Queries the actual execution of the map skeleton's kernel
  ///
  /// \param input  The input container
  ///        args   Additional arguments
  ///
  template <typename... Args>
  void execute(Matrix<Tout>& output,
               const Matrix<IndexPoint>& input,
               Args&&... args) const;
  
  /// \brief Create a program object from the provided source string
  ///
  /// \param source The source code defined by the application developer
  ///
  /// \return A program object customized with the source code defined by
  ///         the application developer, as well as the map skeleton's
  ///         kernel implementation
  ///
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

///
/// \class Map
///
///
template<>
class Map<void(IndexPoint)> : public detail::Skeleton,
                              private detail::MapHelper<void(IndexPoint)> {
public:
  ///
  /// \brief Constructor taking the source code used to customize the Map
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  ///
  ///        funcName Name of the 'main' function (the staring point)
  ///                 of the given source code
  ///
  Map<void(IndexPoint)>(const Source& source,
                        const std::string& funcName = std::string("func"));
  
  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as arguments input and args.
  ///
  /// \param input The input data for the skeleton managed inside a container.
  ///              The actual Type of this argument must be a subtype of the
  ///              Container class.
  ///              If no distribution is set for this container as default the
  ///              Block distribution is selected.
  ///
  ///        args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <typename... Args>
  void operator()(const Matrix<IndexPoint>& input,
                  Args&&... args) const;
  
private:
  /// \brief Queries the actual execution of the map skeleton's kernel
  ///
  /// \param input  The input container
  ///        args   Additional arguments
  ///
  template <typename... Args>
  void execute(const Matrix<IndexPoint>& input,
               Args&&... args) const;
  
  /// \brief Create a program object from the provided source string
  ///
  /// \param source The source code defined by the application developer
  ///
  /// \return A program object customized with the source code defined by
  ///         the application developer, as well as the map skeleton's
  ///         kernel implementation
  ///
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

} // namespace skelcl

// including the definition of the templates
#include "detail/MapDef.h"

#endif // MAP_H_
