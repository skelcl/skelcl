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

__kernel void SCL_STENCIL(__global SCL_TYPE_0* SCL_IN, __global SCL_TYPE_1* SCL_OUT, __global SCL_TYPE_1* SCL_TMP, __local SCL_TYPE_1* SCL_LOCAL_TMP, const unsigned int SCL_TILE_WIDTH,
            const unsigned int SCL_TILE_HEIGHT, const unsigned int SCL_ELEMENTS, const unsigned int SCL_NORTH, const unsigned int SCL_WEST, const unsigned int SCL_SOUTH,
            const unsigned int SCL_EAST, const unsigned int SCL_COLS) {

        const unsigned int col = get_global_id(0);
        const unsigned int l_col = get_local_id(0);
        const unsigned int row = get_global_id(1);
        const unsigned int l_row = get_local_id(1);

        input_matrix_t Mm;
        Mm.data = SCL_LOCAL_TMP;
        Mm.local_row = l_row;
        Mm.local_column = l_col;
        Mm.offset_north = SCL_NORTH;
        Mm.offset_west = SCL_WEST;
        Mm.tile_width = SCL_TILE_WIDTH;

        int i;

        if(l_row==0 && row < SCL_ELEMENTS / SCL_COLS) {
                const unsigned int SCL_ROWS = SCL_ELEMENTS / SCL_COLS;
                const unsigned int SCL_WORKGROUP = SCL_ROWS / get_local_size(1);
                const unsigned int SCL_REST = SCL_ROWS % get_local_size(1);
                const unsigned int SCL_WORKGROUP_X = SCL_COLS / get_local_size(0);
                const unsigned int SCL_REST_X = SCL_COLS % get_local_size(1);

                int withinCols = col < SCL_COLS;
                int withinColsWest = col-SCL_WEST < SCL_COLS;
                int withinColsEast = col+SCL_EAST < SCL_COLS;

                if(row == 0) {
                        for(i = 0; i<SCL_NORTH; i++) {
                                SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = NEUTRAL;
                        }
                        for(i = 0; i<SCL_TILE_HEIGHT-SCL_NORTH; i++){
                                if(withinCols) 	SCL_LOCAL_TMP[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_TMP[i*SCL_COLS+col];
                                else 		SCL_LOCAL_TMP[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST] = NEUTRAL;
                        }

                        if(col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col] = NEUTRAL;
                                }
                        } else if(l_col<SCL_WEST) {
                                for(i = 0; i<SCL_NORTH; i++) {
                                        SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col] = NEUTRAL;
                                }
                                for(i = 0; i<SCL_TILE_HEIGHT-SCL_NORTH; i++){
                                        if(withinColsWest) 	SCL_LOCAL_TMP[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col] = SCL_TMP[i*SCL_COLS+col-SCL_WEST];
                                        else			SCL_LOCAL_TMP[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col] = NEUTRAL;
                                }
                        }
                        if(col>=SCL_COLS-SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++) {
                                        SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                }
                        } else if(l_col>=get_local_size(0) - SCL_EAST) {
                                for(i = 0; i<SCL_NORTH; i++) {
                                        SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                }
                                for(i = 0; i<SCL_TILE_HEIGHT-SCL_NORTH; i++){
                                        if(withinColsEast) 	SCL_LOCAL_TMP[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_TMP[i*SCL_COLS+col+SCL_EAST];
                                        else			SCL_LOCAL_TMP[(i+SCL_NORTH)*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                }
                        }
                } else if(get_group_id(1)==SCL_WORKGROUP && SCL_REST != 0) {
                        unsigned int LAST_WG_TILE_HEIGHT = SCL_TILE_HEIGHT;
                        if(SCL_REST != 0) {
                                LAST_WG_TILE_HEIGHT = SCL_REST + SCL_NORTH;
                        }
                        for(i = 0; i<LAST_WG_TILE_HEIGHT; i++){
                                if(withinCols) 	SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_TMP[(row+i-SCL_NORTH)*SCL_COLS+col];
                                else		SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = NEUTRAL;
                        }
                        for(i = 0; i<SCL_TILE_HEIGHT - LAST_WG_TILE_HEIGHT; i++) {
                                SCL_LOCAL_TMP[(i+LAST_WG_TILE_HEIGHT)*SCL_TILE_WIDTH+l_col+SCL_WEST] = NEUTRAL;
                        }
                        if(col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col] = NEUTRAL;
                                }
                        } else if(l_col<SCL_WEST) {
                                for(i = 0; i<LAST_WG_TILE_HEIGHT; i++) {
                                        if(withinColsWest) 	SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col] = SCL_TMP[(row+i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                        else			SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col] = NEUTRAL;
                                }
                                for(i = 0; i<SCL_TILE_HEIGHT - LAST_WG_TILE_HEIGHT; i++){
                                        SCL_LOCAL_TMP[(i+LAST_WG_TILE_HEIGHT)*SCL_TILE_WIDTH+l_col] = NEUTRAL;
                                }
                        }

                        if(col>=SCL_COLS-SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                }
                        } else if(l_col>=get_local_size(0) - SCL_EAST) {
                                for(i = 0; i<LAST_WG_TILE_HEIGHT; i++) {
                                        if(withinColsEast) 	SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_TMP[(row+i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                        else			SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                }
                                for(i = 0; i<SCL_TILE_HEIGHT - LAST_WG_TILE_HEIGHT; i++){
                                        SCL_LOCAL_TMP[(i+LAST_WG_TILE_HEIGHT)*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                }
                        }
                } else {
                        for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                int withinRows = row + i - SCL_NORTH < SCL_ROWS;
                                if(withinCols && withinRows) 		SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = SCL_TMP[(row+i-SCL_NORTH)*SCL_COLS+col];
                                else					SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST] = NEUTRAL;
                        }

                        if(col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col] = NEUTRAL;
                                }
                        } else if(l_col<SCL_WEST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        int withinRows = row + i - SCL_NORTH < SCL_ROWS;
                                        if(withinColsWest && withinRows) 	SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col] = SCL_TMP[(row+i-SCL_NORTH)*SCL_COLS+col-SCL_WEST];
                                        else					SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col] = NEUTRAL;
                                }
                        }

                        if(col>=SCL_COLS-SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++) {
                                        SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                }
                        } else if(l_col>=get_local_size(0) - SCL_EAST) {
                                for(i = 0; i<SCL_TILE_HEIGHT; i++){
                                        int withinRows = row + i - SCL_NORTH < SCL_ROWS;
                                        if(withinColsEast && withinRows) 	SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = SCL_TMP[(row+i-SCL_NORTH)*SCL_COLS+col+SCL_EAST];
                                        else if(withinColsEast && !withinRows) 	SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                        else if(!withinColsEast && withinRows)	SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                        else					SCL_LOCAL_TMP[i*SCL_TILE_WIDTH+l_col+SCL_WEST+SCL_EAST] = NEUTRAL;
                                }
                        }
                }

        }


        barrier(CLK_LOCAL_MEM_FENCE);

        if(row<SCL_ELEMENTS/SCL_COLS && col<SCL_COLS) {
            SCL_OUT[row*SCL_COLS+col] = USR_FUNC(&Mm);
        }
        barrier(CLK_GLOBAL_MEM_FENCE);

}

)"
