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
/// \file AllPairsKernel.cl
///
/// \author Malte Friese <malte.friese@uni-muenster.de>
///
 
R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;
typedef float SCL_TYPE_2;


__kernel void SCL_ALLPAIRS(const __global SCL_TYPE_0* M,
                           const __global SCL_TYPE_1* N,
                                 __global SCL_TYPE_2* P,
                           const unsigned int dimension,
                           const unsigned int height,
                           const unsigned int width) {
    __local SCL_TYPE_0 Ml[R][D];
    __local SCL_TYPE_1 Nl[D][C];

    const unsigned int   col = get_global_id(0);
    const unsigned int l_col = get_local_id(0);
    const unsigned int   row = get_global_id(1) % R + (get_global_id(1) / R) * R * S;
    const unsigned int l_row = get_local_id(1);

    for (int m = 0; m < S; ++m)
        if ((row + m * R < height) && (col < width))
            P[(row + m * R) * width + col] = SCL_IDENTITY;

    for (int segment = 0; segment < dimension/D; ++segment) {

        uint ii = segment * D / R;
        uint roffset = segment * D - ii * R;
        uint end = (segment + 1) * D;
        for (int i = ii; i * R < end; ++i)
            if (((i - ii) * R + l_row >= roffset) && (i * R + l_row < end) && (col < width))
                Nl[(i - ii) * R + l_row - roffset][l_col] = N[(i * R + l_row) * width + col]; 

        for (int s = 0; s < S; ++s) {
            if ((row + s * R >= height) && (row - l_row + s * R >= height)) break;
            SCL_TYPE_2 result;
            if ((row + s * R < height) && (col < width))
                result = P[(row + s * R) * width + col];

            uint jj = segment * D / C;
            uint coffset = segment * D - jj * C;

            for (int j = jj; j * C < end; ++j)
                if (((j - jj) * C + l_col >= coffset) && (j * C + l_col < end) && (row + s * R < height))
                    Ml[l_row][(j - jj) * C + l_col - coffset] = M[(row + s * R) * dimension + (j * C + l_col)];

            barrier(CLK_LOCAL_MEM_FENCE); 

            for (int k = 0; k < D; ++k) {
                result = USR_REDUCE(result, USR_ZIP(Ml[l_row][k], Nl[k][l_col]));
            }

            if ((row + s * R < height) && (col < width))
                P[(row + s * R) * width + col] = result;

            barrier(CLK_GLOBAL_MEM_FENCE);
        } 
    }
    
    if (dimension % D != 0 && D > 1) {
        
        int segment = dimension/D;

        uint ii = segment * D / R;
        uint roffset = segment * D - ii * R;
        for (int i = ii; i * R < dimension; ++i)
            if (((i - ii) * R + l_row >= roffset) && (i * R + l_row < dimension) && (col < width))
                Nl[(i - ii) * R + l_row - roffset][l_col] = N[(i * R + l_row) * width + col]; 

        for (int s = 0; s < S; ++s) {
            if ((row + s * R >= height) && (row - l_row + s * R >= height)) break;
            SCL_TYPE_2 result;
            if ((row + s * R < height) && (col < width))
                result = P[(row + s * R) * width + col];

            uint jj = segment * D / C;
            uint coffset = segment * D - jj * C;

            for (int j = jj; j * C < dimension; ++j)
                if (((j - jj) * C + l_col >= coffset) && (j * C + l_col < dimension) && (row + s * R < height))
                    Ml[l_row][(j - jj) * C + l_col - coffset] = M[(row + s * R) * dimension + (j * C + l_col)];

            barrier(CLK_LOCAL_MEM_FENCE); 

            for (int k = 0; k < dimension % D; ++k) {
                result = USR_REDUCE(result, USR_ZIP(Ml[l_row][k], Nl[k][l_col]));
            }

            if ((row + s * R < height) && (col < width))
                P[(row + s * R) * width + col] = result;

            barrier(CLK_GLOBAL_MEM_FENCE);
        } 
    }
}
)"
