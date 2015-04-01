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
///

R"(

#define localIndex(rowOffset, colOffset) (((rowOffset + SCL_L_ROW) + SCL_NORTH) * SCL_TILE_WIDTH + SCL_WEST + (SCL_L_COL + colOffset))
#define globalIndex(rowOffset, colOffset) ((rowOffset + SCL_ROW) * SCL_COLS + (colOffset + SCL_COL))

__kernel void SCL_STENCIL(__global SCL_TYPE_1* SCL_IN,
                          __global SCL_TYPE_1* SCL_OUT,
                          __global SCL_TYPE_0* SCL_INITIAL,
                          __local SCL_TYPE_1* SCL_LOCAL,
                          const unsigned int SCL_ELEMENTS,
                          const unsigned int SCL_COLS)
{
  SCL_TYPE_0 neutral = NEUTRAL;
  input_matrix_t Mm;
  Mm.data = SCL_LOCAL;

  const unsigned int col = get_global_id(0);
  const unsigned int l_col = get_local_id(0);
  const unsigned int row = get_global_id(1);
  const unsigned int l_row = get_local_id(1);

  int i;


        if(l_row==0 && row < SCL_ELEMENTS / SCL_COLS) {
                const unsigned int SCL_ROWS_ = SCL_ELEMENTS / SCL_COLS;

                int withinCols = col < SCL_COLS;
                int withinColsWest = col-SCL_WEST < SCL_COLS;
                int withinColsEast = col+SCL_EAST < SCL_COLS;

                if(row == 0) {
                        for(i = 0; i<SCL_NORTH; i++) {
                                SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = neutral;
                        }
                        for(i = SCL_NORTH; i < SCL_TILE_HEIGHT; i++) {
                                if(withinCols) 	SCL_LOCAL[i * SCL_TILE_WIDTH + l_col + SCL_WEST] = SCL_IN[(i - SCL_NORTH) * SCL_COLS + col];
                                else 		SCL_LOCAL[i * SCL_TILE_WIDTH + l_col + SCL_WEST] = neutral;
                        }

                        if(col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                                }
                        } else if(l_col<SCL_WEST) {
                                for(i = 0; i<SCL_NORTH; i++) {
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                                }
                                for(i = SCL_NORTH; i<SCL_TILE_HEIGHT; i++){
                                        if(withinColsWest) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN[(i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                        else			SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                                }
                        }


                        if(col>=SCL_COLS-SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++) {
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                                }
                        } else if(l_col>=get_local_size(0) - SCL_EAST) {
                                for(i = 0; i<SCL_NORTH; i++) {
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                                }
                                for(i = SCL_NORTH; i<SCL_TILE_HEIGHT; i++){
                                        if(withinColsEast) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN[(i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                        else			SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                                }
                        }
                } else if(row - SCL_NORTH + SCL_TILE_HEIGHT < SCL_ROWS_) {
                    for(i = 0; i<SCL_TILE_HEIGHT; i++){
                            if(withinCols) 		SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN[(row+i-SCL_NORTH)*SCL_COLS+col];
                            else			SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = neutral;
                    }

                    if(col<SCL_WEST) {
                            for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                    SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                            }
                    } else if(l_col<SCL_WEST) {
                            for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                    if(withinColsWest) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN[(row+i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                    else		SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                            }
                    }

                    if(col>=SCL_COLS-SCL_EAST) {
                            for(i = 0; i<SCL_TILE_HEIGHT; i++) {
                                    SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                            }
                    } else if(l_col>=get_local_size(0) - SCL_EAST) {
                            for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                    if(withinColsEast) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN[(row+i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                    else		SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                            }
                    }
                } else {
                        for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                int withinRows = row + i - SCL_NORTH < SCL_ROWS_;
                                if(withinCols && withinRows) 		SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_IN[(row+i-SCL_NORTH)*SCL_COLS+col];
                                else					SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = neutral;
                        }

                        if(col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                                }
                        } else if(l_col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        int withinRows = row + i - SCL_NORTH < SCL_ROWS_;
                                        if(withinColsWest && withinRows) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = SCL_IN[(row+i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                        else					SCL_LOCAL[i*SCL_TILE_WIDTH+l_col] = neutral;
                                }
                        }

                        if(col>=SCL_COLS-SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++) {
                                        SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                                }
                        } else if(l_col>=get_local_size(0) - SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        int withinRows = row + i - SCL_NORTH < SCL_ROWS_;
                                        if(withinColsEast && withinRows) 	SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_IN[(row+i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                        else					SCL_LOCAL[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = neutral;
                                }
                        }
                }

        }

  barrier(CLK_LOCAL_MEM_FENCE);

  if (SCL_ROW < SCL_ROWS && SCL_COL < SCL_COLS) {
    SCL_OUT[globalIndex(0, 0)] = USR_FUNC(&Mm);
  }
}

)"
