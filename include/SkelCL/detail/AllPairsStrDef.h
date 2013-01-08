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
/// \file AllPairsStrDef.h
///
///	\author Malte Friese <malte.friese@uni-muenster.de>
///

#ifndef ALLPAIRS2_STR_DEF_H
#define ALLPAIRS2_STR_DEF_H

#include <algorithm>
#include <istream>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#undef __CL_ENABLE_EXCEPTIONS

#include <ssedit/TempSourceFile.h>
#include <ssedit/Function.h>

#include "../Distributions.h"
#include "../Matrix.h"
#include "../Reduce.h"
#include "../Zip.h"
#include "../Out.h"

#include "Assert.h"
#include "Device.h"
#include "KernelUtil.h"
#include "Logger.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

namespace skelcl {

// Konstruktor
template<typename Tleft, typename Tright, typename Tout>
AllPairsStr<Tout(Tleft, Tright)>::AllPairsStr(const std::string& source, const std::string& func)
    : _srcUser(source),
      _funcUser(func)
{
    this->_C = 16;
    this->_R = 16;
    this->_S = 1;

    LOG_DEBUG("Create new AllPairs object (", this, ")");
}

// Programm erstellen
template<typename Tleft, typename Tright, typename Tout>
detail::Program AllPairsStr<Tout(Tleft, Tright)>::createAndBuildProgram() const
{
    ASSERT_MESSAGE( !_srcUser.empty(),
                    "Tried to create program with empty user source." );

    // create program
    std::string s(Matrix<Tout>::deviceFunctions());

    // helper structs and functions
    s.append(R"(
typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;
typedef float SCL_TYPE_2;

typedef struct {
    const __global SCL_TYPE_0* data;
    unsigned int dimension;
    unsigned int row;
} lmatrix_t;

typedef struct {
    const __global SCL_TYPE_1* data;
    unsigned int width;
    unsigned int column;
} rmatrix_t;

SCL_TYPE_0 getElementFromRow(lmatrix_t *matrix, const unsigned int element_id) {
    return matrix->data[(matrix->row) * matrix->dimension + element_id];
}

SCL_TYPE_1 getElementFromColumn(rmatrix_t *matrix, const unsigned int element_id) {
    return matrix->data[element_id * (matrix->width) + matrix->column];
}

)");

    // user source
    s.append(_srcUser);

    // allpairs skeleton source
    s.append(
      #include "AllPairsKernel2.cl"
    );

    auto program = detail::Program(s, detail::util::hash("//AllPairs\n"
                                                         + Matrix<Tout>::deviceFunctions()
                                                         + _srcUser
                                                         + _funcUser));
    // modify program
    if (!program.loadBinary()) {
        program.transferParameters(_funcUser, 3, "SCL_ALLPAIRS");
        program.transferArguments(_funcUser, 3, "USR_FUNC");

        program.renameFunction(_funcUser, "USR_FUNC");

        program.adjustTypes<Tleft, Tright, Tout>();
    }

    program.build();

    return program;
}

} // namespace skelcl

#endif // ALLPAIRS_STR_DEF_H
