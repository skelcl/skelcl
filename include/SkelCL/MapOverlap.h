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
/// \file MapOverlap.h
///
///	\author Stefan Breuer<s_breu03@uni-muenster.de>
///
#ifndef MapOverlap_H_
#define MapOverlap_H_

#include <istream>
#include <string>

#include "detail/Padding.h"
#include "detail/Skeleton.h"
#include "detail/Program.h"

namespace skelcl {

/// \cond
/// Don't show this forward declarations in doxygen
template <typename> class Matrix;
template <typename> class Out;

template <typename> class MapOverlap;
/// \endcond

///
/// \defgroup mapOverlap MapOverlap Skeleton
///
/// \brief The MapOverlap skeleton describes calculations performed on one or
///        more devices. It invokes a unary user-defined function on a container
///        in a parallel fashion. Unlike the Map skeleton the user-defined
///        function can access more than just a single element of the container.
///
/// \ingroup skeletons
///

///
/// \brief This class implements the MapOverlap skeleton, which describes
///        calculations performed on one or more devices. It invokes a unary
///        user-defined function on a container in a parallel fashion. Unlike
///        the Map skeleton the user-defined function can access more than just
///        a single element of the container.
///
/// On creation the MapOverlap skeleton is customized with source code defining
/// a unary function, an overlap range, as well as a Padding mode. The overlap
/// range specifies how many elements of the neighborhood of an element can be
/// accesses by the user-defined function. The Padding mode defines how out of
/// bound accesses are handled.
///
/// More formally: When c is a container of length n with items c[0] .. c[n-1],
/// f is the provided unary function, and r is the overlap range, the MapOverlap
/// skeleton performs the calculation f(c[i-r], .., c[i], .., c[i+r]) for
/// every i in 0 .. n-1. If an out of bound access of c occurs the Padding mode
/// defines the behavior.
///
/// As all skeletons, the MapOverlap skeleton allows for passing additional
/// arguments, i.e. arguments besides the input container, to the user defined
/// function.
/// The user-defined function has to written in such a way, that it expects more
/// than just one argument (it is no unary function any more). Accordingly the
/// number, types and order of arguments used when calling the map skeleton has
/// to match the declaration of the user-defined function.
///
/// \tparam Tin   The type of the elements stored in the input container.
/// \tparam Tout  The type of the elements stored in the output container.
///
/// \ingroup skeletons
/// \ingroup mapOverlap
///
template <typename Tin, typename Tout>
class MapOverlap<Tout(Tin)> : public detail::Skeleton {

public:
  /// 
  /// \brief Constructor taking the source code used of the user-defined
  ///        function, the overlap range, and the Padding mode as arguments.
  ///
  /// \param source   The source code of the user-defined function. 
  /// \param overlap_range The number of elements to be accessible for the
  ///                      user-defined function in each direction.
  /// \param padding  The Padding mode for handling out of bound accesses.
  /// \param neutral_element The neutral element used by some Padding modes.
  /// \param func     The name of the user-defined function which should be
  ///                 invoked by the Map skeleton.
  ///
  MapOverlap<Tout(Tin)>(const Source& source, unsigned int overlap_range = 1,
                        detail::Padding padding = detail::Padding::NEAREST,
                        Tin neutral_element = Tin(),
                        const std::string& func = std::string("func"));
  
  /// 
  /// \brief Executes the skeleton on the provided input Matrix. The
  ///        resulting data is stored in a newly created output Matrix and
  ///        the Matrix is returned.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param in     The input Matrix on which the user-defined function is
  ///               invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \return A newly created Matrix storing the elements which get computed
  ///         after invoking the user-defined function on the input Matrix
  ///         and the additionally provided arguments.
  /// 
  template <typename... Args>
  Matrix<Tout> operator()(const Matrix<Tin>& in, Args&&... args);


  /// 
  /// \brief Executes the skeleton on the provided input Matrix. The
  ///        resulting data is stored in the provided output Matrix and a
  ///        reference to this Matrix is returned.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param output The output Matrix in which the resulting data is stored.
  ///               The type of this argument is not the type of the Matrix
  ///               itself, but instead a wrapping class tagging that this
  ///               container is written into. The utility function
  ///               skelcl::out() can be used to create this wrapper for an
  ///               arbitrary container.
  /// \param in     The input Matrix on which the user-defined function is
  ///               invoked.
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \return A newly created Matrix storing the elements which get computed
  ///         after invoking the user-defined function on the input Matrix
  ///         and the additionally provided arguments.
  /// 
  template <typename... Args>
  Matrix<Tout>& operator()(Out<Matrix<Tout>> output, const Matrix<Tin>& in,
                           Args&&... args);

private:
  template <typename... Args>
  void execute(Matrix<Tout>& output, const Matrix<Tin>& in, Args&&... args);

  detail::Program createAndBuildProgram() const;

  void prepareInput(const Matrix<Tin>& in);

  void prepareOutput(Matrix<Tout>& output, const Matrix<Tin>& in);

  std::string _userSource;
  std::string _funcName;
  unsigned int _overlap_range;
  detail::Padding _padding;
  Tin _neutral_element;
  detail::Program _program;
};

} //namespace skelcl

#include "detail/MapOverlapDef.h"

#endif /* MAPOVERLAP_H_ */
