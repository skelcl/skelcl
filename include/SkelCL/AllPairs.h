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
///

#ifndef ALLPAIRS_H
#define ALLPAIRS_H

#include <istream>
#include <string>

#include "detail/Skeleton.h"

namespace skelcl {

template <typename> class Matrix;
template <typename> class Reduce;
template <typename> class Zip;
template <typename> class Out;
namespace detail { class Program; }

template<typename> class AllPairs;

///
/// \class AllPairs
///
/// \brief An instance of the AllPairs skeleton describes a calculation of all two
///        pairs which can be performed on a device.
///
/// \tparam Tleft  Type of the left input data of the skeleton
///         Tright Type of the right input data of the skeleton
///         Tout   Type of the output data of the skeleton
///
/// On creation the AllPairs skeleton is either customized with a given reduce and
/// zip skeleton or with a user defined source code as string.
/// The AllPairs skeleton can then be called by passing two matrix containers.
/// The AllPairs skeleton will compute the results of all two pairs of the row
/// and column-vectors from the matrices and store them in a result matrix.
/// More formally: When M and N are two matrices of dimension height x dimension
/// and dimension x width. Then D is the result matrix of dimension height x width.
/// Every D[i,j] is the scalar result from either Reduce(Zip(M_i, N_j)) or
/// UserDefString(M_i, N_j), where M_i is the ith row in M and N_j is the jth column in N.
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
    ///
    ///        zip      Zip skeleton
    ///
    ///        funcName Name of the 'main' function (the starting point)
    ///                 of the given source code
    ///
    AllPairs<Tout(Tleft, Tright)>(const Reduce<Tout(Tout)>& reduce, const Zip<Tout(Tleft, Tright)>& zip);

    ///
    /// \brief Constructor taking the source code used to customize the
    ///        AllPairs skeleton.
    ///
    /// \param source   Source code used to customize the skeleton
    ///
    ///        funcName Name of the 'main' function (the starting point)
    ///                 of the given source code
    ///
    AllPairs<Tout(Tleft, Tright)>(const std::string& source, const std::string& func = std::string("func"));

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
    ///              distribution is set for the right container the implementation
    ///              will try to select a suitable distribution for allpairs computations
    ///
    ///        right The second (right) input data for the skeleton managed inside
    ///              a container. The actual Type of this argument must be a
    ///              subtype of the Container class.
    ///              If no distribution is set for this container as well as for
    ///              the left container as default the Copy distribution is
    ///              selected.
    ///              If no distribution is set for this container and a
    ///              distribution is set for the left container the implementation
    ///              will try to select a suitable distribution for allpairs computations
    ///
    ///        args  Additional arguments which are passed to the function
    ///              named by funcName and defined in the source code at created.
    ///              The individual arguments must be passed in the same order
    ///              here as they were defined in the funcName function
    ///              declaration.
    ///
    template <typename... Args>
    Matrix<Tout> operator()(const Matrix<Tleft>& left,
                            const Matrix<Tright>& right,
                            Args&&... args);

    ///
    /// \brief Function call operator. Executes the skeleton on the data provided
    ///        as arguments left, right  and args. The resulting data is stored
    ///        in the provided container output. A reference to the output
    ///        container is returned to allow for chaining skeleton calls.
    ///
    /// \param left  The first (left) input data for the skeleton managed inside
    ///              a container. The actual Type of this argument must be a
    ///              subtype of the Container class.
    ///              If no distribution is set for this container as well as for
    ///              the right container as default the Block distribution is
    ///              selected.
    ///              If no distribution is set for this container and a
    ///              distribution is set for the right container the implementation
    ///              will try to select a suitable distribution for allpairs computations
    ///
    ///        right The second (right) input data for the skeleton managed inside
    ///              a container. The actual Type of this argument must be a
    ///              subtype of the Container class.
    ///              If no distribution is set for this container as well as for
    ///              the left container as default the Copy distribution is
    ///              selected.
    ///              If no distribution is set for this container and a
    ///              distribution is set for the left container the implementation
    ///              will try to select a suitable distribution for allpairs computations
    ///
    ///        args  Additional arguments which are passed to the function
    ///              named by funcName and defined in the source code at created.
    ///              The individual arguments must be passed in the same order
    ///              here as they were defined in the funcName function
    ///              declaration.
    ///
    template <typename... Args>
    Matrix<Tout>& operator()(Out< Matrix<Tout> > output,
                             const Matrix<Tleft>& left,
                             const Matrix<Tright>& right,
                             Args&&... args);

    private:
    ///
    /// \brief Queries the actual execution of the allpairs skeleton's kernel
    ///
    /// \param output The ouput container
    ///        left   The first (left) input container
    ///        right  The second (right) input container
    ///        args   Additional arguments
    ///
    template <typename... Args>
    void execute(const detail::Program& program,
                 Matrix<Tout>& output,
                 const Matrix<Tleft>& left,
                 const Matrix<Tright>& right,
                 Args&&... args);

    ///
    /// \brief Create a program object
    ///
    /// \return A program object customized with either the given reduce and zip skeletons
    ///         or by a user defined function by the application developer, as well as
    ///         the allpairs skeleton's kernel implementation
    ///
    detail::Program createAndBuildProgram() const;

    ///
    /// \brief Create a program object from a given reduce skeleton and zip skeleton
    ///
    /// \return A program object customized with the given reduce and zip skeletons
    ///         as well as the allpairs skeleton's kernel implementation
    ///
    detail::Program createAndBuildProgramSpecial() const;

    ///
    /// \brief Create a program object from the provided user defined source string
    ///
    /// \return A program object customized with the source code defined by
    ///         the application developer as well as the allpairs skeleton's
    ///         kernel implementation
    ///
    detail::Program createAndBuildProgramGeneral() const;

    ///
    /// \brief Prepares the inputs for kernel execution
    ///
    /// \param left  The first (left) input container to be prepared
    ///        right The second (right) input container to be prepared
    ///
    void prepareInput(const Matrix<Tleft>& left,
                      const Matrix<Tright>& right);

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
    void prepareOutput(Matrix<Tout>& output,
                       const Matrix<Tleft>& left,
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
    bool _isSpecial;
    unsigned int _C;
    unsigned int _R;
    unsigned int _S;
};

} // namespace skelcl

#include "detail/AllPairsDef.h"

#endif // ALLPAIRS_H
