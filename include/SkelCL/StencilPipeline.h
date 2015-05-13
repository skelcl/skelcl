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
/// \file StencilPipeline.h
///
/// A sequential pipeline for iterative stencil operations.
///
///	\author Stefan Breuer<s_breu03@uni-muenster.de>
///     \author Chris Cummins <chrisc.101@gmail.com>
///
#ifndef STENCILPIPELINE_H_
#define STENCILPIPELINE_H_

#include <istream>
#include <string>
#include <vector>

#include "./detail/Padding.h"
#include "./detail/Skeleton.h"
#include "./detail/Program.h"

#include "./Source.h"
#include "./Stencil.h"

namespace skelcl {

template<typename > class Matrix;
template<typename > class Out;

template<typename > class StencilPipeline;

template<typename Tin, typename Tout>
class StencilPipeline<Tout(Tin)> : public detail::Skeleton {

public:
	StencilPipeline<Tout(Tin)>(const int iterBetweenSwaps = 1);

	// Add a new stencil to the pipeline.
	void add(Stencil<Tout(Tin)> stencil);

        // Run pipeline.
	template<typename ... Args>
	Matrix<Tout> operator()(const unsigned int iterations,
                                const Matrix<Tin>& input,
                                Args&&... args);

	template<typename ... Args>
        Matrix<Tout>& operator()(const unsigned int iterations,
                                 const Matrix<Tin>& input,
                                 Out<Matrix<Tout>> output,
                                 Args&&... args);

	template<typename ... Args>
        Matrix<Tout>& operator()(const unsigned int iterations,
                                 const Matrix<Tin>& input,
                                 Out<Matrix<Tout>> temp,
                                 Out<Matrix<Tout>> output,
                                 Args&&... args);

private:
	template<typename ... Args>
        void execute(const Matrix<Tin>& input,
                     Matrix<Tout>& temp,
                     Matrix<Tout>& output,
                     Args&&... args);

	void prepareInput(const Matrix<Tin>& in);
	void prepareOutput(Matrix<Tout>& output, const Matrix<Tin>& in);

        unsigned int determineIterationsBetweenDataSwaps(const Matrix<Tin> &input,
                                                         unsigned int iterLeft);

        unsigned int determineNorthSum();
        unsigned int determineEastSum();
        unsigned int determineSouthSum();
        unsigned int determineWestSum();

	std::vector<Stencil<Tout(Tin)>> _pipeline;

	int _iterBetweenSwaps;
        int _iterations;
};

} //namespace skelcl

#include "detail/StencilPipelineDef.h"

#endif  // STENCILPIPELINE_H_
