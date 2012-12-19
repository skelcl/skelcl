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
/// \file AllPairsKernel2.cl
///
/// \author Malte Friese <malte.friese@uni-muenster.de>
///
 
R"(

SCL_TYPE_0 getElementFromRow(lmatrix_t *matrix, const unsigned int element_id) {
    return matrix->data[(matrix->row) * matrix->dimension + element_id];
}

SCL_TYPE_1 getElementFromColumn(rmatrix_t *matrix, const unsigned int element_id) {
    return matrix->data[element_id * (matrix->width) + matrix->column];
}

__kernel void SCL_ALLPAIRS2(const __global SCL_TYPE_0* M,
                            const __global SCL_TYPE_1* N,
                                  __global SCL_TYPE_2* P,
                            const unsigned int dimension,
                            const unsigned int height,
                            const unsigned int width) {

    const unsigned int col = get_global_id(0);
    const unsigned int row = get_global_id(1);

    lmatrix_t Mm;
    Mm.data = M;
    Mm.dimension = dimension;
    Mm.row = row;

    rmatrix_t Nm;
    Nm.data = N;
    Nm.width = width;
    Nm.column = col;

    if (row < height && col < width) {
        P[row * width + col] = USR_FUNC(&Mm, &Nm, dimension);
    }
}
)"
