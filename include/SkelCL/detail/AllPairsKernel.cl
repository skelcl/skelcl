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

#define R 8
#define C 32
#define SUBTILES 4
#define DS 32

__kernel void SCL_ALLPAIRS(__global float* M,
                           __global float* N,
                           __global float* P, 
                           int height, int width) {
    __local float Ml[R][DS];
    __local float Nl[DS][C];

    const uint   col = get_global_id(0);
    const uint l_col = get_local_id(0);
    const uint   row = get_global_id(1) % R + (get_global_id(1) / R) * R * SUBTILES;
    const uint l_row = get_local_id(1);

    uint segment = 0;

    while (segment * DS < width) { 

        uint ii = segment * DS / R; 
        for (int i = ii; i * R < (segment + 1) * DS; ++i) 
            Nl[(i - ii) * R + l_row][l_col] = N[(i * R + l_row) * width + col]; 

        for (int s = 0; s < SUBTILES; ++s) { 
            float sum = P[(row + s * R) * width + col]; 

            uint jj = segment * DS / C; 
            for (int j = jj; j * C < (segment + 1) * DS; ++j) 
                Ml[l_row][(j - jj) * C + l_col] = M[(row + s * R) * width + (j * C + l_col)]; 

            barrier(CLK_LOCAL_MEM_FENCE); 

            for (int k = 0; k < DS; ++k) \
                sum += Ml[l_row][k] * Nl[k][l_col]; 

            P[(row + s * R) * width + col] = sum; 

            barrier(CLK_GLOBAL_MEM_FENCE);	

        } 
        ++segment; 
    } 
}
)"
