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
/// \file StencilKernel.cl
///
/// \author Stefan Breuer <s_breu03@uni-muenster.de>
/// \author Chris Cummins <chrisc.101@gmail.com>
///

R"(

__kernel void SCL_STENCIL(__global SCL_TYPE_1* SCL_IN,
                          __global SCL_TYPE_1* SCL_OUT,
                          __global SCL_TYPE_0* SCL_INITIAL,
                          __local SCL_TYPE_1* SCL_LOCAL,
                          const unsigned int SCL_ELEMENTS,
                          const unsigned int SCL_COLS) {
#if STENCIL_PADDING_NEUTRAL
        SCL_TYPE_0 neutral = NEUTRAL;
#endif

// For the \"nearest initial\" padding type, we need to distinguish between the
// current input and initial input. For \"nearest\" padding type, we don't need
// this distinction, so we can alias the two inputs to the same value. This
// massively reduces the amount of conditional code between nearest and nearest
// initial padding types.
#if STENCIL_PADDING_NEAREST_INITIAL
#  define SCL_IN_A SCL_IN
#  define SCL_IN_B SCL_INITIAL
#else
#  define SCL_IN_A SCL_IN
#  define SCL_IN_B SCL_IN
#endif

        const unsigned int col = get_global_id(0);
        const unsigned int l_col = get_local_id(0);
        const unsigned int row = get_global_id(1);
        const unsigned int l_row = get_local_id(1);

        input_matrix_t Mm;
        Mm.data = SCL_LOCAL;
        int i;

        if(l_row==0 && row < SCL_ELEMENTS / SCL_COLS) {
                const unsigned int SCL_WORKGROUP = SCL_ROWS / get_local_size(1);
                const unsigned int SCL_REST = SCL_ROWS % get_local_size(1);

                int withinCols = col < SCL_COLS;
                int withinColsWest = col-SCL_WEST < SCL_COLS;
                int withinColsEast = col+SCL_EAST < SCL_COLS;

                if(row == 0) {
                        for(i = 0; i<SCL_NORTH; i++) {
#if STENCIL_PADDING_NEUTRAL
                                SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = neutral;
#else
                                if(withinCols)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_B[col];
                                else            SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_B[SCL_COLS-1];
#endif
                        }
#if STENCIL_PADDING_NEUTRAL
                        for(i = SCL_NORTH; i < SCL_TILE_HEIGHT; i++) {
                                if(withinCols) 	SCL_LOCAL[i * SCL_TILE_WIDTH + l_col + SCL_WEST] = SCL_IN_A[(i - SCL_NORTH) * SCL_COLS + col];
                                else 		SCL_LOCAL[i * SCL_TILE_WIDTH + l_col + SCL_WEST] = neutral;
                        }
#else
                        for(i = 0; i<SCL_TILE_HEIGHT-SCL_NORTH; i++){
                                if(withinCols)  SCL_LOCAL[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_A[i*SCL_COLS+col];
                                else            SCL_LOCAL[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_B[(i+1)*SCL_COLS-1];
                        }
#endif

                        if(col<SCL_WEST) {
#if STENCIL_PADDING_NEUTRAL
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                                }
#else
                                for(i = 0; i<SCL_NORTH; i++){
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[0];
                                }
#endif

#if !STENCIL_PADDING_NEUTRAL
                                for(i = 0; i<SCL_TILE_HEIGHT-SCL_NORTH; i++){
                                        SCL_LOCAL[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col] = SCL_IN_A[i*SCL_COLS];
                                }
#endif

                        } else if(l_col<SCL_WEST) {
#if STENCIL_PADDING_NEUTRAL
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                                }
                                for(i = SCL_NORTH; i<SCL_TILE_HEIGHT; i++){
                                        if(withinColsWest) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_A[(i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                        else			SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                                }
#else
                                for(i = 0; i<SCL_NORTH; i++) {
                                        if(withinColsWest)      SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[col-SCL_WEST];
                                        else                    SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[SCL_COLS-1];
                                }
                                for(i = 0; i<SCL_TILE_HEIGHT-SCL_NORTH; i++){
                                        if(withinColsWest)      SCL_LOCAL[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col] = SCL_IN_A[i*SCL_COLS+col-SCL_WEST];
                                        else                    SCL_LOCAL[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col] = SCL_IN_B[(i+1)*SCL_COLS-1];
                                }
#endif
                        }
                        if(col>=SCL_COLS-SCL_EAST) {
#if STENCIL_PADDING_NEUTRAL
                                for(i = 0; i<SCL_TILE_HEIGHT; i++) {
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                                }
#else
                                for(i = 0; i<SCL_NORTH; i++) {
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[SCL_COLS-1];
                                }
                                for(i = 0; i<SCL_TILE_HEIGHT - SCL_NORTH; i++) {
                                        SCL_LOCAL[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[(i+1)*SCL_COLS-1];
                                }
#endif
                        } else if(l_col>=get_local_size(0) - SCL_EAST) {
                                for(i = 0; i<SCL_NORTH; i++) {
#if STENCIL_PADDING_NEUTRAL
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
#else
                                        if(withinColsEast)      SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[col+SCL_EAST];
                                        else                    SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[SCL_COLS-1];
#endif
                                }
#if STENCIL_PADDING_NEUTRAL
                                for(i = SCL_NORTH; i<SCL_TILE_HEIGHT; i++){
                                        if(withinColsEast) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_A[(i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                        else			SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                                }
#else
                                for(i = 0; i<SCL_TILE_HEIGHT - SCL_NORTH; i++){
                                        if(withinColsEast)      SCL_LOCAL[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_A[i*SCL_COLS+col+SCL_EAST];
                                        else                    SCL_LOCAL[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[(i+1)*SCL_COLS-1];
                                }
#endif
                        }
                } else if (row - SCL_NORTH + SCL_TILE_HEIGHT < SCL_ROWS) {
                        for(i = 0; i<SCL_TILE_HEIGHT; i++){
#if STENCIL_PADDING_NEUTRAL
                                if(withinCols) 		SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col];
                                else			SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = neutral;
#else
                                if(withinCols)          SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col];
                                else                    SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_B[(row+i+1-SCL_NORTH)*SCL_COLS-1];
#endif
                        }

                        if(col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
#if STENCIL_PADDING_NEUTRAL
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
#else
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[(row+i-SCL_NORTH)*SCL_COLS];
#endif
                                }
                        } else if(l_col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
#if STENCIL_PADDING_NEUTRAL
                                    if(withinColsWest) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                    else		SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
#else
                                    if(withinColsWest)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                    else                SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[(row+i+1-SCL_NORTH)*SCL_COLS-1];
#endif
                                }
                        }

                        if(col>=SCL_COLS-SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++) {
#if STENCIL_PADDING_NEUTRAL
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
#else
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[(row+1+i-SCL_NORTH)*SCL_COLS-1];
#endif
                                }
                        } else if(l_col>=get_local_size(0) - SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
#if STENCIL_PADDING_NEUTRAL
                                    if(withinColsEast) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                    else		SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
#else
                                    if(withinColsEast)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                    else                SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[(row+i+1-SCL_NORTH)*SCL_COLS-1];
#endif
                                }
                        }
                }
                else {
                        for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                int withinRows = row + i - SCL_NORTH < SCL_ROWS;
#if STENCIL_PADDING_NEUTRAL
                                if(withinCols && withinRows) 		SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col];
                                else					SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = neutral;
#else
                                if(withinCols && withinRows)            SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col];
                                else if(withinCols && !withinRows)      SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_B[SCL_ELEMENTS-SCL_COLS+col];
                                else if(!withinCols && withinRows)      SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_B[(row+1+i-SCL_NORTH)*SCL_COLS-1];
                                else                                    SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN_B[SCL_ELEMENTS-1];
#endif
                        }

                        if(col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
#if STENCIL_PADDING_NEUTRAL
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
#else
                                        int withinRows = row + i - SCL_NORTH < SCL_ROWS;
                                        if(withinRows)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[(row+i-SCL_NORTH)*SCL_COLS];
                                        else            SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[SCL_ELEMENTS-SCL_COLS];
#endif
                                }
                        } else if(l_col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        int withinRows = row + i - SCL_NORTH < SCL_ROWS;
#if STENCIL_PADDING_NEUTRAL
                                        if(withinColsWest && withinRows) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                        else					SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
#else
                                        if(withinColsWest && withinRows)        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                        else if(withinColsWest && !withinRows)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[SCL_ELEMENTS-SCL_COLS+col-SCL_WEST];
                                        else if(!withinColsWest && withinRows)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[(row+1+i-SCL_NORTH)*SCL_COLS-1];
                                        else                                    SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN_B[SCL_ELEMENTS-1];
#endif
                                }
                        }

                        if(col>=SCL_COLS-SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++) {
#if STENCIL_PADDING_NEUTRAL
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
#else
                                        int withinRows = row + i - SCL_NORTH < SCL_ROWS;
                                        if(withinRows)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[(row+1+i-SCL_NORTH)*SCL_COLS-1];
                                        else            SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[SCL_ELEMENTS-1];
# endif
                                }
                        } else if(l_col>=get_local_size(0) - SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        int withinRows = row + i - SCL_NORTH < SCL_ROWS;
#if STENCIL_PADDING_NEUTRAL
                                        if(withinColsEast && withinRows) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                        else					SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
#else
                                        if(withinColsEast && withinRows)        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_A[(row+i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                        else if(withinColsEast && !withinRows)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[SCL_ELEMENTS-SCL_COLS+col+SCL_EAST];
                                        else if(!withinColsEast && withinRows)  SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[(row+1+i-SCL_NORTH)*SCL_COLS-1];
                                        else                                    SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN_B[SCL_ELEMENTS-1];
#endif
                                }
                        }
                }
        }

// Clear up after ourselves.
#undef SCL_IN_A
#undef SCL_IN_B

        barrier(CLK_LOCAL_MEM_FENCE);

        if(row<SCL_ELEMENTS/SCL_COLS && col<SCL_COLS) {
            SCL_OUT[row*SCL_COLS+col] = USR_FUNC(&Mm);
        }
}

)"
