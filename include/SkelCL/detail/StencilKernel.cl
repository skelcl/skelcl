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

__kernel void SCL_STENCIL(__global SCL_TYPE_0* SCL_IN, __global SCL_TYPE_1* SCL_OUT, __global SCL_TYPE_1* SCL_TMP, __local SCL_TYPE_0* SCL_LOCAL_TEMP, const unsigned int SCL_ELEMENTS,
                const unsigned int SCL_NORTH, const unsigned int SCL_WEST, const unsigned int SCL_SOUTH, const unsigned int SCL_EAST, const unsigned int SCL_LARGESTNORTH, const unsigned int SCL_COLS) {

#ifdef NEUTRAL
        const unsigned int col = get_global_id(0);
        const unsigned int l_col = get_local_id(0);
        const unsigned int row = get_global_id(1);
        const unsigned int l_row = get_local_id(1);

        input_matrix_t Mm;
        Mm.data = SCL_LOCAL_TEMP;
        Mm.local_row = l_row;
        Mm.local_column = l_col;
        Mm.offset_north = SCL_NORTH;
        Mm.offset_west = SCL_WEST;

        int i,j,k,l,m;

        if(l_row==0) {
            const unsigned int SCL_ROWS = SCL_ELEMENTS / SCL_COLS;
            const unsigned int SCL_WORKGROUP = SCL_ROWS / get_local_size(1);

            if(row == 0) {
                for(m=0;m<SCL_NORTH;m++) {
                    SCL_LOCAL_TEMP[m*TILE_WIDTH+l_col+SCL_WEST] = 22;
                }
                for(m=SCL_NORTH;m<TILE_HEIGHT;m++){
                    SCL_LOCAL_TEMP[m*TILE_WIDTH+l_col+SCL_WEST] = 999;
                }
            } else if(get_group_id(1)==SCL_WORKGROUP-1){
                for(m=0;m<TILE_HEIGHT-SCL_SOUTH;m++) {
                        SCL_LOCAL_TEMP[m*TILE_WIDTH+l_col+SCL_WEST] = SCL_TMP[(row+m)*SCL_COLS+col];
                        //SCL_SHARED[m][l_col+SCL_WEST] = SCL_TMP[(row+m)*SCL_COLS+col];
                }
                for(m=TILE_HEIGHT-SCL_SOUTH;m<TILE_HEIGHT;m++) {
                        SCL_LOCAL_TEMP[m*TILE_WIDTH+l_col+SCL_WEST] = NEUTRAL;
                        //SCL_SHARED[m][l_col+SCL_WEST] = SCL_TMP[(row+m)*SCL_COLS+col];
                }
            } else {
                //Fill columns of local memory in which the mapped elements reside
                for(m=0;m<TILE_HEIGHT;m++) {
                        SCL_LOCAL_TEMP[m*TILE_WIDTH+l_col+SCL_WEST] = SCL_TMP[(row+m)*SCL_COLS+col];
                        //SCL_SHARED[m][l_col+SCL_WEST] = SCL_TMP[(row+m)*SCL_COLS+col];
                }
            }
            //Fill columns of local memory left of the mapped elements
            if(l_col<SCL_WEST) {
                    for(i=0;i<TILE_HEIGHT;i++) {
                         SCL_LOCAL_TEMP[i*TILE_WIDTH+l_col] = SCL_TMP[(row+i)*SCL_COLS+col-SCL_WEST];
                        //SCL_SHARED[i][l_col] = SCL_TMP[(row+i)*SCL_COLS+col-SCL_WEST];
                    }
            }
            //Fill columns of local memory left of the mapped elements when padding elements to the left are needed
            if(col<SCL_WEST) {
                    for(j=0;j<TILE_HEIGHT;j++) {
                        SCL_LOCAL_TEMP[j*TILE_WIDTH+l_col] = NEUTRAL;
                        //SCL_SHARED[j][l_col] = NEUTRAL;
                    }
            }
            //Fill columns of local memory right of the mapped elements
            if(l_col>=get_local_size(0)-SCL_EAST) {
                    for(k=0;k<TILE_HEIGHT;k++) {
                        SCL_LOCAL_TEMP[k*TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_TMP[(row+k)*SCL_COLS+col+SCL_EAST];
                        //SCL_SHARED[k][l_col+SCL_WEST+SCL_EAST] = SCL_TMP[(row+k)*SCL_COLS+col+SCL_EAST];
                    }
            }
            //Fill columns of local memory right of the mapped elements when padding elements to the right are needed
            if(col>=SCL_COLS-SCL_EAST) {
                    for(l=0;l<TILE_HEIGHT;l++) {
                        SCL_LOCAL_TEMP[k*TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                        //SCL_SHARED[l][l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                    }
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);

        if(row<SCL_ELEMENTS/SCL_COLS && col<SCL_COLS) {
                //Working with global mem
                //SCL_OUT[row*SCL_COLS+col+SCL_COLS*SCL_NORTH] = USR_FUNC(&(SCL_TMP[row*SCL_COLS+col+SCL_NORTH*SCL_COLS]));
                //Working with local mem
                //SCL_OUT[row*SCL_COLS+col+SCL_NORTH*SCL_COLS] = USR_FUNC(&(SCL_SHARED[l_row+SCL_NORTH][l_col+SCL_WEST]));
                SCL_OUT[row*SCL_COLS+col+SCL_LARGESTNORTH*SCL_COLS] = USR_FUNC(&Mm);

        }

#else
        const unsigned int col = get_global_id(0);
        const unsigned int l_col = get_local_id(0);
        const unsigned int row = get_global_id(1);
        const unsigned int l_row = get_local_id(1);

        __local SCL_TYPE_0 SCL_SHARED[TILE_WIDTH][TILE_WIDTH];

        int i,j,k,l,m;

        if(l_row==0) {
                //Fill columns of local memory in which the mapped elements reside
                for(m=0;m<TILE_WIDTH;m++) {
                        SCL_SHARED[m][l_col+SCL_OVERLAP] = SCL_IN[(row+m)*SCL_COLS+col];
                }
                //Fill columns of local memory left of the mapped elements
                if(l_col<SCL_OVERLAP) {
                        for(i=0;i<TILE_WIDTH;i++) {
                                SCL_SHARED[i][l_col] = SCL_IN[(row+i)*SCL_COLS+col-SCL_OVERLAP];
                        }
                }
                //Fill columns of local memory left of the mapped elements when padding elements to the left are needed
                if(col<SCL_OVERLAP) {
                        for(j=0;j<TILE_WIDTH;j++) {
                                SCL_SHARED[j][l_col] = SCL_IN[(row+j)*SCL_COLS];
                        }
                }
                //Fill columns of local memory right of the mapped elements
                if(l_col>=get_local_size(0)-SCL_OVERLAP) {
                        for(k=0;k<TILE_WIDTH;k++) {
                                SCL_SHARED[k][l_col+SCL_OVERLAP+SCL_OVERLAP] = SCL_IN[(row+k)*SCL_COLS+col+SCL_OVERLAP];
                        }
                }
                //Fill columns of local memory right of the mapped elements when padding elements to the right are needed
                if(col>=SCL_COLS-SCL_OVERLAP) {
                        for(l=0;l<TILE_WIDTH;l++) {
                                SCL_SHARED[l][l_col+SCL_OVERLAP+SCL_OVERLAP] = SCL_IN[(row+l)*SCL_COLS+SCL_COLS-1];
                        }
                }

        }
        barrier(CLK_LOCAL_MEM_FENCE);

        if(row<SCL_ELEMENTS/SCL_COLS && col<SCL_COLS) {
                //Working with global mem
                //SCL_OUT[row*SCL_COLS+col+SCL_OVERLAP*SCL_COLS] = USR_FUNC(&(SCL_IN[row*SCL_COLS+col+SCL_OVERLAP*SCL_COLS]));
                 //Working with local mem
                SCL_OUT[row*SCL_COLS+col++SCL_OVERLAP*SCL_COLS] = USR_FUNC(&(SCL_SHARED[l_row+SCL_OVERLAP][l_col+SCL_OVERLAP]));
        }
#endif
}

)"
