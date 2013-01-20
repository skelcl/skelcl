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

/// 
/// \brief This class implements the Map skeleton, which describes calculations
///        performed on one or more devices. It invokes a unary used-defined
///        function on a container in a parallel fashion.
///
/// \tparam The Map class takes one template argument describing the
///         declaration of the user-defined function.
///
/// On creation the Map skeleton is customized with source code defining a
/// unary function. An instance of this class acts like a function object,
/// i.e. it can be called like a function. When this happens a container is
/// passed as input to the Map skeleton which then invokes the provided
/// function on every element of the container in a parallel fashion on one or
/// more devices (depending of the distribution of the container).
///
/// More formally: When c is a container of length n with items c[0] .. c[n-1],
/// and f is the provided unary function, the Map skeleton performs the
/// calculation f(x[i]) for ever i in 0 .. n-1.
///
/// Several versions of this skeleton exist, depending on the declaration of the
/// user defined function. Some of them produce an output, while others do not.
///
/// As all skeletons, the Map skeleton allows for passing additional arguments,
/// i.e. arguments besides the input container, to the user defined function.
/// The user-defined function has to written in such a way, that it expects more
/// than just one argument (it is no unary function any more). Accordingly the
/// number, types and order of arguments used when calling the map skeleton has
/// to match the declaration of the user-defined function.
/// 
template<typename> class Map;

/// 
/// \brief This is the most general version of the Map skeleton.
///        It can be customized with a user-defined function taking an arbitrary
///        types argument (no classes are currently possible) and produces an
///        arbitrary typed output.
///
/// \tparam Tin   The type of the elements stored in the input container.
/// \tparam Tout  The type of the elements stored in the output container.
/// 
template<typename Tin, typename Tout>
class Map<Tout(Tin)> : public detail::Skeleton,
                       private detail::MapHelper<Tout(Tin)> {
public:
  /// 
  /// \brief Constructor taking the source code used of the user-defined
  ///        function as argument.
  ///
  /// \param source   The source code of the user-defined function. 
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton
  /// 
  Map<Tout(Tin)>(const Source& source,
                 const std::string& funcName = std::string("func"));

  /// 
  /// \brief Executes the skeleton on the provided input container. The
  ///        resulting data is stored in a newly created output container and
  ///        the container is returned.
  ///
  /// \tparam C     The incomplete type of the container used as input and
  ///               output. The complete types are C<Tin> for the input and
  ///               C<Tout> for the output.
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param input  The input container on which the user-defined function is
  ///               invoked
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \return A newly created container storing the elements which get computed
  ///         after invoking the user-defined function on the input container
  ///         and the additionally provided arguments.
  /// 
  template <template <typename> class C,
            typename... Args>
  C<Tout> operator()(const C<Tin>& input,
                     Args&&... args) const;

  /// 
  /// \brief Executes the skeleton on the provided input container. The
  ///        resulting data is stored in the provided output container and a
  ///        reference to this container is returned.
  ///
  /// \tparam C     The incomplete type of the container used as input and
  ///               output. The complete types are C<Tin> for the input and
  ///               C<Tout> for the output.
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param output The output container in which the resulting data is stored.
  ///               The type of this argument is not the type of the container
  ///               itself, but instead a wrapping class tagging that this
  ///               container is written into. The utility function skelcl::out
  ///               can be used to create this wrapper for an arbitrary
  ///               container.
  /// \param input  The input container on which the user-defined function is
  ///               invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \return       A reference to the provided output container. This container
  ///               contains the elements which gets computed after invoking the
  ///               user-defined function on the input container and the
  ///               additionally provided arguments.
  /// 
  template <template <typename> class C,
            typename... Args>
  C<Tout>& operator()(Out<C<Tout>> output,
                      const C<Tin>&  input,
                      Args&&... args) const;

private:
  /// 
  /// \brief Starts the execution of the map skeleton's kernel.
  ///
  /// \tparam C     The incomplete type of the container used as input and
  ///               output. The complete types are C<Tin> for the input and
  ///               C<Tout> for the output.
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param output A reference to the output container in which the resulting
  ///               data is stored.
  /// \param input  The input container on which the user-defined function is
  ///               invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  /// 
  template <template <typename> class C,
            typename... Args>
  void execute(C<Tout>& output,
               const C<Tin>& input,
               Args&&... args) const;

  /// 
  /// \brief  Utility function creating a program for the Map skeleton from the
  ///         source code (represented as a string) and the function name as
  ///         provided to the constructor of this class.
  ///
  /// \param source   The source code defining the user-defined function of the
  ///                 Map skeleton represented as a string.
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  ///
  /// \return A valid program object which has already been built for the
  ///         available devices. The program contains the kernel which gets
  ///         executed inside the execute function of this class. If the source
  ///         code defined by the user is not valid this function might
  ///         terminate the execution of the program and present the built log
  ///         of the used OpenCL implementation.
  /// 
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

/// 
/// \brief  This version of the Map skeleton is used, when the user-defined
///         function has the return type void, i.e. it doesn't produce a return
///         value.
///
/// \tparam Tin The type of the elements stored in the input container.
///
template<typename Tin>
class Map<void(Tin)> : public detail::Skeleton,
                       private detail::MapHelper<void(Tin)> {
public:
  /// 
  /// \brief Constructor taking the source code used of the user-defined
  ///        function as argument.
  ///
  /// \param source   The source code of the user-defined function. 
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton
  /// 
  Map<void(Tin)>(const Source& source,
                 const std::string& funcName = std::string("func"));

  /// 
  /// \brief Executes the skeleton on the provided input container.
  ///
  /// \tparam C     The incomplete type of the input container. The complete
  ///               type is C<Tin>.
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param input  The input container on which the user-defined function is
  ///               invoked
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  /// 
  template <template <typename> class C,
            typename... Args>
  void operator()(const C<Tin>& input,
                  Args&&... args) const;

private:
  /// 
  /// \brief Starts the execution of the map skeleton's kernel.
  ///
  /// \tparam C     The incomplete type of the input container. The complete
  ///               type is C<Tin>.
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param input  The input container on which the user-defined function is
  ///               invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  /// 
  template <template <typename> class C,
            typename... Args>
  void execute(const C<Tin>& input,
               Args&&... args) const;

  /// 
  /// \brief  Utility function creating a program for the Map skeleton from the
  ///         source code (represented as a string) and the function name as
  ///         provided to the constructor of this class.
  ///
  /// \param source   The source code defining the user-defined function of the
  ///                 Map skeleton represented as a string.
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  ///
  /// \return A valid program object which has already been built for the
  ///         available devices. The program contains the kernel which gets
  ///         executed inside the execute function of this class. If the source
  ///         code defined by the user is not valid this function might
  ///         terminate the execution of the program and present the built log
  ///         of the used OpenCL implementation.
  /// 
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

/// 
/// \brief  This version of the Map skeleton is executed over an one-dimensional
///         index space defined by an IndexVector. It can be customized with a
///         user-defined function taking an Index (an integer value) and
///         producing an arbitrary typed output for every element of the index
///         space.
///
/// \tparam Tout  The type of the elements stored in the output Vector.
/// 
template<typename Tout>
class Map<Tout(Index)> : public detail::Skeleton,
                         private detail::MapHelper<Tout(Index)> {
public:
  /// 
  /// \brief Constructor taking the source code used of the user-defined
  ///        function as argument.
  ///
  /// \param source   The source code of the user-defined function. 
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  /// 
  Map<Tout(Index)>(const Source& source,
                   const std::string& funcName = std::string("func"));

  /// 
  /// \brief Executes the skeleton on the index space described by the input
  ///        Vector. The resulting data is stored in a newly created output 
  ///        Vector and the Vector is returned.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  ///
  /// \param input  The input Vector describing an index space  on which the
  ///               user-defined function is invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  ///
  /// \return       A newly created Vector storing the elements which get
  ///               computed after invoking the user-defined function on the
  ///               input Vector and the additionally provided arguments.
  /// 
  template <typename... Args>
  Vector<Tout> operator()(const Vector<Index>& input,
                          Args&&... args) const;

  /// 
  /// \brief Executes the skeleton on the index space described by the input
  ///        Vector. The resulting data is stored in the provided output
  ///        Vector and a reference to this Vector is returned.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  ///
  /// \param output The output Vector in which the resulting data is stored.
  ///               The type of this argument is not the type of the Vector
  ///               itself, but instead a wrapping class tagging that this
  ///               Vector is written into. The utility function skelcl::out
  ///               can be used to create this wrapper for an arbitrary
  ///               Vector.
  /// \param input  The input Vector describing an index space on which the
  ///               user-defined function is invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  ///
  /// \return       A reference to the provided output Vector. This Vector
  ///               contains the elements which get computed after invoking the
  ///               user-defined function on the input Vector and the
  ///               additionally provided arguments.
  /// 
  template <typename... Args>
  Vector<Tout>& operator()(Out<Vector<Tout>> output,
                           const Vector<Index>& input,
                           Args&&... args) const;

private:
  /// 
  /// \brief Starts the execution of the map skeleton's kernel.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  ///
  /// \param output A reference to the output Vector in which the resulting
  ///               data is stored.
  /// \param input  The input Vector describing an index space on which the
  ///               user-defined function is invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  /// 
  template <typename... Args>
  void execute(Vector<Tout>& output,
               const Vector<Index>& input,
               Args&&... args) const;

  /// 
  /// \brief  Utility function creating a program for the Map skeleton from the
  ///         source code (represented as a string) and the function name as
  ///         provided to the constructor of this class.
  ///
  /// \param source   The source code defining the user-defined function of the
  ///                 Map skeleton represented as a string.
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  ///
  /// \return A valid program object which has already been built for the
  ///         available devices. The program contains the kernel which gets
  ///         executed inside the execute function of this class. If the source
  ///         code defined by the user is not valid this function might
  ///         terminate the execution of the program and present the built log
  ///         of the used OpenCL implementation.
  /// 
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

/// 
/// \brief  This version of the Map skeleton is executed over an one-dimensional
///         index space defined by an IndexVector. It can be customized with a
///         user-defined function taking an Index (an integer value). For this
///         version the user-defined function must be void, i.e. return nothing.
/// 
template<>
class Map<void(Index)> : public detail::Skeleton,
                         private detail::MapHelper<void(Index)> {
public:
  /// 
  /// \brief Constructor taking the source code used of the user-defined
  ///        function as argument.
  ///
  /// \param source   The source code of the user-defined function. 
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  /// 
  Map<void(Index)>(const Source& source,
                   const std::string& funcName = std::string("func"));

  /// 
  /// \brief  Executes the skeleton on the index space described by the input
  ///         Vector.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  ///
  /// \param input  The input Vector describing an index space on which the
  ///               user-defined function is invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  /// 
  template <typename... Args>
  void operator()(const Vector<Index>& input,
                  Args&&... args) const;

private:
  /// 
  /// \brief Starts the execution of the map skeleton's kernel.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  ///
  /// \param input  The input Vector describing an index space on which the
  ///               user-defined function is invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input Vector.
  /// 
  template <typename... Args>
  void execute(const Vector<Index>& input,
               Args&&... args) const;

  /// 
  /// \brief  Utility function creating a program for the Map skeleton from the
  ///         source code (represented as a string) and the function name as
  ///         provided to the constructor of this class.
  ///
  /// \param source   The source code defining the user-defined function of the
  ///                 Map skeleton represented as a string.
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  ///
  /// \return A valid program object which has already been built for the
  ///         available devices. The program contains the kernel which gets
  ///         executed inside the execute function of this class. If the source
  ///         code defined by the user is not valid this function might
  ///         terminate the execution of the program and present the built log
  ///         of the used OpenCL implementation.
  /// 
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

/// 
/// \brief  This version of the Map skeleton is executed over an two-dimensional
///         index space defined by an IndexMatrix. It can be customized with a
///         user-defined function taking an IndexPoint (a pair of integer
///         values) and producing an arbitrary typed output for every element of
///         the index space.
///
/// \tparam Tout  The type of the elements stored in the output Matrix.
/// 
template<typename Tout>
class Map<Tout(IndexPoint)> : public detail::Skeleton,
                              private detail::MapHelper<Tout(IndexPoint)> {
public:
  /// 
  /// \brief Constructor taking the source code used of the user-defined
  ///        function as argument.
  ///
  /// \param source   The source code of the user-defined function. 
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  /// 
  Map<Tout(IndexPoint)>(const Source& source,
                        const std::string& funcName = std::string("func"));
  
  /// 
  /// \brief Executes the skeleton on the index space described by the input
  ///        Matrix. The resulting data is stored in a newly created output 
  ///        Matrix and the Matrix is returned.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input Matrix.
  ///
  /// \param input  The input Matrix describing an index space on which the
  ///               user-defined function is invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input Matrix.
  ///
  /// \return       A newly created Matrix storing the elements which get
  ///               computed after invoking the user-defined function on the
  ///               input Matrix and the additionally provided arguments.
  /// 
  template <typename... Args>
  Matrix<Tout> operator()(const Matrix<IndexPoint>& input,
                          Args&&... args) const;
  
  /// 
  /// \brief Executes the skeleton on the index space described by the input
  ///        Matrix. The resulting data is stored in the provided output
  ///        Matrix and a reference to this Matrix is returned.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input Matrix.
  ///
  /// \param output The output Matrix in which the resulting data is stored.
  ///               The type of this argument is not the type of the Matrix
  ///               itself, but instead a wrapping class tagging that this
  ///               Matrix is written into. The utility function skelcl::out
  ///               can be used to create this wrapper for an arbitrary
  ///               Matrix.
  /// \param input  The input Matrix describing an index space on which the
  ///               user-defined function is invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input Matrix.
  ///
  /// \return       A reference to the provided output Matrix. This Matrix
  ///               contains the elements which get computed after invoking the
  ///               user-defined function on the input Matrix and the
  ///               additionally provided arguments.
  /// 
  template <typename... Args>
  Matrix<Tout>& operator()(Out<Matrix<Tout>> output,
                           const Matrix<IndexPoint>& input,
                           Args&&... args) const;
  
private:
  /// 
  /// \brief Starts the execution of the map skeleton's kernel.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input Matrix.
  ///
  /// \param output A reference to the output Matrix in which the resulting
  ///               data is stored.
  /// \param input  The input Matrix describing an index space on which the
  ///               user-defined function is invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input Matrix.
  /// 
  template <typename... Args>
  void execute(Matrix<Tout>& output,
               const Matrix<IndexPoint>& input,
               Args&&... args) const;
  
  /// 
  /// \brief  Utility function creating a program for the Map skeleton from the
  ///         source code (represented as a string) and the function name as
  ///         provided to the constructor of this class.
  ///
  /// \param source   The source code defining the user-defined function of the
  ///                 Map skeleton represented as a string.
  /// \param funcName The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  ///
  /// \return A valid program object which has already been built for the
  ///         available devices. The program contains the kernel which gets
  ///         executed inside the execute function of this class. If the source
  ///         code defined by the user is not valid this function might
  ///         terminate the execution of the program and present the built log
  ///         of the used OpenCL implementation.
  /// 
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

template<>
class Map<void(IndexPoint)> : public detail::Skeleton,
                              private detail::MapHelper<void(IndexPoint)> {
public:
  Map<void(IndexPoint)>(const Source& source,
                        const std::string& funcName = std::string("func"));
  
  template <typename... Args>
  void operator()(const Matrix<IndexPoint>& input,
                  Args&&... args) const;
  
private:
  template <typename... Args>
  void execute(const Matrix<IndexPoint>& input,
               Args&&... args) const;
  
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& funcName) const;
};

} // namespace skelcl

// including the definition of the templates
#include "detail/MapDef.h"

#endif // MAP_H_
