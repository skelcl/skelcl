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
/// \file MapOverlapKernel.cl
///
/// \author Stefan Breuer <s_breu03@uni-muenster.de>
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

R"(
#define  localIndex(rowOffset, colOffset) (((rowOffset + l_row) + SCL_OVERLAP_RANGE) * SCL_TILE_WIDTH + SCL_OVERLAP_RANGE + (l_col + colOffset))
#define globalIndex(rowOffset, colOffset) ( (rowOffset + row) * SCL_COLS + (colOffset + col + (SCL_OVERLAP_RANGE * SCL_COLS)))

__kernel void SCL_MAPOVERLAP(__global SCL_TYPE_0* SCL_IN,
                             __global SCL_TYPE_1* SCL_OUT,
                             __local SCL_TYPE_1* SCL_SHARED,
                             const unsigned int SCL_ELEMENTS,
                             const unsigned int SCL_COLS)
{

#ifdef NEUTRAL
        const unsigned int col = get_global_id(0);
        const unsigned int l_col = get_local_id(0);
        const unsigned int row = get_global_id(1);
        const unsigned int l_row = get_local_id(1);

        input_matrix_t Mm;
        Mm.data = SCL_SHARED;
        Mm.local_row = l_row;
        Mm.local_column = l_col;

        int i,j,k,l,m;

	// everybody copies one item
	SCL_SHARED[localIndex(0,0)] = SCL_IN[globalIndex(0,0)];

	// the first row copies the north region
	if (l_row == 0) {
		for (m = 0; m < SCL_OVERLAP_RANGE; m++) {
			SCL_SHARED[localIndex(-m,0)] = SCL_IN[globalIndex(-m,0)];
		}
	}

	// the last row copies the south region
	if (l_row == get_local_size(1) - 1) {
		for (m = 0; m < SCL_OVERLAP_RANGE; m++) {
			SCL_SHARED[localIndex(m,0)] = SCL_IN[globalIndex(m,0)];
		}
	}

	// the second row copies the west and east regions
	if (l_row == 1) {
                //Fill columns of local memory left of the mapped elements when padding elements to the left are needed
		if (col < SCL_OVERLAP_RANGE) {
                        for(i = -SCL_OVERLAP_RANGE; i < get_local_size(1) + SCL_OVERLAP_RANGE; i++) {
                            SCL_SHARED[localIndex(i-1,-SCL_OVERLAP_RANGE)] = NEUTRAL;
                        }
                }
                //Fill columns of local memory left of the mapped elements
		else if (l_col < SCL_OVERLAP_RANGE) {
                        for(i = -SCL_OVERLAP_RANGE; i < get_local_size(1) + SCL_OVERLAP_RANGE; i++) {
				SCL_SHARED[localIndex(i-1,-SCL_OVERLAP_RANGE)] = SCL_IN[globalIndex(i-1,-SCL_OVERLAP_RANGE)];
                        }
                }

                //Fill columns of local memory right of the mapped elements when padding elements to the right are needed
                if (col >= SCL_COLS - SCL_OVERLAP_RANGE) {
                        for(i = -SCL_OVERLAP_RANGE; i < get_local_size(1) + SCL_OVERLAP_RANGE; i++) {
				SCL_SHARED[localIndex(i-1,+SCL_OVERLAP_RANGE)] = NEUTRAL;
                        }
                }
                //Fill columns of local memory right of the mapped elements
                else if( l_col >= get_local_size(0) - SCL_OVERLAP_RANGE) {
                        for(i = -SCL_OVERLAP_RANGE; i < get_local_size(1) + SCL_OVERLAP_RANGE; i++) {
				SCL_SHARED[localIndex(i-1,+SCL_OVERLAP_RANGE)] = SCL_IN[globalIndex(i-1,+SCL_OVERLAP_RANGE)];
                        }
                }
	}

        barrier(CLK_LOCAL_MEM_FENCE);

        if(row<SCL_ELEMENTS/SCL_COLS && col<SCL_COLS) {
                SCL_OUT[globalIndex(0,0)] = USR_FUNC(&Mm);
        }

#else
#if 0
        const unsigned int col = get_global_id(0);
        const unsigned int l_col = get_local_id(0);
        const unsigned int row = get_global_id(1);
        const unsigned int l_row = get_local_id(1);

        input_matrix_t Mm;
        Mm.data = SCL_SHARED;
        Mm.local_row = l_row;
        Mm.local_column = l_col;
        Mm.offset_north = SCL_OVERLAP;
        Mm.offset_west = SCL_OVERLAP;
        Mm.tile_width = SCL_TILE_WIDTH;

        int i,j,k,l,m;

        if(l_row==0 && row<SCL_ELEMENTS/SCL_COLS && col<SCL_COLS) {
                //Fill columns of local memory in which the mapped elements reside
                for(m=0;m<SCL_TILE_WIDTH;m++) {
                    SCL_SHARED[m*SCL_TILE_WIDTH+l_col+SCL_OVERLAP] = SCL_IN[(row+m)*SCL_COLS+col];
                    //SCL_SHARED[m][l_col+SCL_OVERLAP] = SCL_IN[(row+m)*SCL_COLS+col];
                }
                //Fill columns of local memory left of the mapped elements
                if(l_col<SCL_OVERLAP) {
                        for(i=0;i<SCL_TILE_WIDTH;i++) {
                            SCL_SHARED[i*SCL_TILE_WIDTH+l_col] = SCL_IN[(row+i)*SCL_COLS+col-SCL_OVERLAP];
                            //SCL_SHARED[i][l_col] = SCL_IN[(row+i)*SCL_COLS+col-SCL_OVERLAP];
                        }
                }
                //Fill columns of local memory left of the mapped elements when padding elements to the left are needed
                if(col<SCL_OVERLAP) {
                        for(j=0;j<SCL_TILE_WIDTH;j++) {
                            SCL_SHARED[j*SCL_TILE_WIDTH+l_col] = SCL_IN[(row+j)*SCL_COLS];
                            //SCL_SHARED[j][l_col] = SCL_IN[(row+j)*SCL_COLS];
                        }
                }
                //Fill columns of local memory right of the mapped elements
                if(l_col>=get_local_size(0)-SCL_OVERLAP) {
                        for(k=0;k<SCL_TILE_WIDTH;k++) {
                            SCL_SHARED[k*SCL_TILE_WIDTH+l_col+SCL_OVERLAP+SCL_OVERLAP] = SCL_IN[(row+k)*SCL_COLS+col+SCL_OVERLAP];
                            //SCL_SHARED[k][l_col+SCL_OVERLAP+SCL_OVERLAP] = SCL_IN[(row+k)*SCL_COLS+col+SCL_OVERLAP];
                        }
                }
                //Fill columns of local memory right of the mapped elements when padding elements to the right are needed
               if(col>=SCL_COLS-SCL_OVERLAP) {
                        for(l=0;l<SCL_TILE_WIDTH;l++) {
                             SCL_SHARED[l*SCL_TILE_WIDTH+l_col+SCL_OVERLAP+SCL_OVERLAP] = SCL_IN[(row+l)*SCL_COLS-1+SCL_COLS];
                        }
                }

        }
        barrier(CLK_LOCAL_MEM_FENCE);

        if(row<SCL_ELEMENTS/SCL_COLS && col<SCL_COLS) {
            SCL_OUT[row*SCL_COLS+col+SCL_OVERLAP*SCL_COLS] = USR_FUNC(&Mm);
        }
#endif
#endif
}

)"

