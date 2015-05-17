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
/// \file AllPairs.h
///
///	\author Malte Friese <malte.friese@uni-muenster.de>
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef ALLPAIRS_H
#define ALLPAIRS_H

#include <istream>
#include <string>

#include "detail/Skeleton.h"
#include "detail/Program.h"

namespace skelcl {

/// \cond
/// Don't show this forward declarations in doxygen
template <typename> class Matrix;
template <typename> class Reduce;
template <typename> class Zip;
template <typename> class Out;

template<typename> class AllPairs;
/// \endcond

///
/// \defgroup allpairs AllPairs Skeleton
///
/// \brief The AllPairs skeleton describes a calculation of pairs of row vectors
///        and column vectors with a user defined function which can be
///        performed on one or more devices.
///
/// \ingroup skeletons
///

///
/// \brief An instance of the AllPairs class describes a calculation of
///        pairs of row vectors and column vectors with a user defined function
///        which can be performed on a device.
///
/// On creation the AllPairs skeleton is customized with either a given
/// Reduce<T(T)> and Zip<Tout(Tleft, Tright)> skeleton or with source code
/// defining a function. The source code function header receives
/// two vectors of the same length and the length as an integer as follows:
/// \code
/// Tout func(lmatrix_t* row, rmatrix_t* col, const unsigned int dim).
/// \endcode
/// The i-th element of the row-vector row of type \c lmatrix_t can be
/// accessed via the API-Function \c getElementFromRow(row, i) and the i-th
/// element of the column-vector col of type \c rmatrix_t can be accessed via
/// the API-function \c getElementFromColumn(col, i).
///
/// The AllPairs skeleton can be executed by passing two matrix containers.
/// The AllPairs skeleton will compute the results of all pairs of row-vectors
/// from the left matrix and column-vectors from the right matrix and store them
/// in a result matrix.
/// More formally: \c M is a matrix of size \c heigt \c x \c dim and \c N
/// is a matrix of size \c dim \c x \c width and \c F is the customized function
/// that receives a row-vector from \c M, a column-vector from \c N and
/// calculates a scalar value.
/// Then \c P is the result matrix of size \c height \c x \c width.
/// Every \c P[i,j] is the scalar result from \c F(M_i, N_j), where \c M_i is
/// the ith row vector in \c M for i in [0,..,\c height-1] and \c N_j is the
/// jth column vector in \c N for j in [0,...,\c width-1].
/// When a Zip<Tout(Tleft, Tright)> and a Reduce<T(T)> skeleton is used to
/// customize the allpairs skeleton, then function \c F is
/// \c Reduce(Zip(M_i,N_j)).
/// Passing a Zip<Tout(Tleft, Tright)> and Reduce<T(T)> skeleton yields a better
/// runtime than defining a function as source code, as local memory is used
/// automatically.
///
/// \tparam Tleft  Type of the left input data of the skeleton.
/// \tparam Tright Type of the right input data of the skeleton.
/// \tparam Tout   Type of the output data of the skeleton.
///
/// \ingroup skeletons
/// \ingroup allpairs
///
template<typename Tleft,
         typename Tright,
         typename Tout>
class AllPairs<Tout(Tleft, Tright)> : public detail::Skeleton {
public:
  ///
  /// \brief Constructor taking a Reduce and a Zip skeleton used to
  ///        customize the AllPairs skeleton.
  ///
  /// \param reduce   Reduce skeleton
  /// \param zip      Zip skeleton
  ///
  AllPairs<Tout(Tleft, Tright)>(const Reduce<Tout(Tout)>& reduce,
                                const Zip<Tout(Tleft, Tright)>& zip);

  ///
  /// \brief Constructor taking the source code used to customize the
  ///        AllPairs skeleton.
  ///
  /// \param source   Source code used to customize the skeleton
  /// \param func     Name of the 'main' function (the starting point)
  ///                 of the given source code
  ///
  AllPairs<Tout(Tleft, Tright)>(const std::string& source,
                                const std::string& func = std::string("func"));

  ///
  /// \brief Executes the skeleton on the data provided as arguments left, right
  ///        and args. The resulting data is stored in a newly created output
  ///        matrix.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param left  The first (left) input data for the skeleton managed inside
  ///              a matrix.
  ///              If no distribution is set for this container as well as for
  ///              the right container as default the Block distribution is
  ///              selected.
  ///              If no distribution is set for this container and a
  ///              distribution is set for the right container the implementation
  ///              will try to select a suitable distribution for allpairs
  ///              computations.
  ///
  ///  \param right The second (right) input data for the skeleton managed
  ///               inside a matrix.
  ///               If no distribution is set for this matrix as well as for
  ///               the left matrix as default the Copy distribution is
  ///               selected.
  ///               If no distribution is set for this matrix and a
  ///               distribution is set for the left matrix the implementation
  ///               will try to select a suitable distribution for allpairs
  ///               computations
  ///
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \return A newly created Matrix storing the elements which get computed
  ///         by performing the allpairs computation.
  ///
  template <typename... Args>
  Matrix<Tout> operator()(const Matrix<Tleft>& left,
                          const Matrix<Tright>& right, Args&&... args);

  ///
  /// \brief Executes the skeleton on the provided containers. The
  ///        resulting data is stored in the provided output container and a
  ///        reference to this container is returned.
  ///
  /// \tparam Args  The types of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  ///
  /// \param output The output matrix in which the resulting data is stored.
  ///               The type of this argument is not the type of the matrix
  ///               itself, but instead a wrapping class tagging that this
  ///               container is written into. The utility function skelcl::out
  ///               can be used to create this wrapper for an arbitrary
  ///               container.
  ///
  ///  \param left The first (left) input data for the skeleton managed inside
  ///              a matrix.
  ///              If no distribution is set for this matrix as well as for
  ///              the right matrix as default the Block distribution is
  ///              selected.
  ///              If no distribution is set for this matrix and a
  ///              distribution is set for the right matrix the implementation
  ///              will try to select a suitable distribution for allpairs
  ///              computations
  ///
  ///  \param right The second (right) input data for the skeleton managed
  ///               inside a matrix.
  ///               If no distribution is set for this matrix as well as for
  ///               the left matrix as default the Copy distribution is
  ///               selected.
  ///               If no distribution is set for this matrix and a
  ///               distribution is set for the left matrix the implementation
  ///               will try to select a suitable distribution for allpairs
  ///               computations
  ///
  /// \param args   The values of the arguments which are passed to the
  ///               user-defined function in addition to the input container.
  //
  /// \return       A reference to the provided output Matrix. This Matrix
  ///               contains the elements which gets computed by performing the
  ///               allpairs computation.
  ///
  template <typename... Args>
  Matrix<Tout>& operator()(Out< Matrix<Tout> > output,
                           const Matrix<Tleft>& left,
                           const Matrix<Tright>& right,
                           Args&&... args);

private:
  template <typename... Args>
  void execute(Matrix<Tout>& output, const Matrix<Tleft>& left,
               const Matrix<Tright>& right, Args&&... args);

  detail::Program createAndBuildProgramSpecial() const;

  detail::Program createAndBuildProgramGeneral() const;

  void prepareInput(const Matrix<Tleft>& left, const Matrix<Tright>& right);

  void prepareOutput(Matrix<Tout>& output, const Matrix<Tleft>& left,
                     const Matrix<Tright>& right);

  // used by special implementation
  std::string _srcReduce;
  std::string _srcZip;
  std::string _funcReduce;
  std::string _funcZip;
  std::string _idReduce;

  // used by general implementation
  std::string _srcUser;
  std::string _funcUser;

  // used by both implementations
  unsigned int _C;
  unsigned int _R;
  unsigned int _S;
  detail::Program _program;
};

} // namespace skelcl

#include "detail/AllPairsDef.h"

#endif // ALLPAIRS_H
