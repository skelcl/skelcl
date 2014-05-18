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
#pragma OPENCL EXTENSION cl_amd_printf:enable
int localIndex(int rowOffset, int colOffset)
{
  const unsigned int l_col = get_local_id(0);
  const unsigned int l_row = get_local_id(1);
  return ((rowOffset + l_row) + SCL_OVERLAP_RANGE)
       * SCL_TILE_WIDTH + SCL_OVERLAP_RANGE + (l_col + colOffset);
}

int globalIndex(int rowOffset, int colOffset, int SCL_COLS)
{
  const unsigned int col = get_global_id(0);
  const unsigned int row = get_global_id(1);
  return (rowOffset + row)
       * SCL_COLS + (colOffset + col + (SCL_OVERLAP_RANGE * SCL_COLS));
}

__kernel void SCL_MAPOVERLAP(__global SCL_TYPE_0* SCL_IN,
                             __global SCL_TYPE_1* SCL_OUT,
                             __local SCL_TYPE_1* SCL_SHARED,
                             const unsigned int SCL_ELEMENTS,
                             const unsigned int SCL_COLS)
{
  const unsigned int col = get_global_id(0);
  const unsigned int l_col = get_local_id(0);
  const unsigned int row = get_global_id(1);
  const unsigned int l_row = get_local_id(1);

  input_matrix_t Mm;
  Mm.data = SCL_SHARED;
  Mm.local_row = l_row;
  Mm.local_column = l_col;

  int i, m;

  // everybody copies one item
  SCL_SHARED[localIndex(0, 0)] = SCL_IN[globalIndex(0, 0, SCL_COLS)];

  barrier(CLK_LOCAL_MEM_FENCE);

  // the first row copies the north region
  if (l_row == 0) {
    for (m = 1; m <= SCL_OVERLAP_RANGE; m++) {
      SCL_SHARED[localIndex(-m, 0)] = SCL_IN[globalIndex(-m, 0, SCL_COLS)];
    }
  }

  // the last row copies the south region
  if (l_row == get_local_size(1) - 1) {
    for (m = 1; m <= SCL_OVERLAP_RANGE; m++) {
      SCL_SHARED[localIndex(m, 0)] = SCL_IN[globalIndex(m, 0, SCL_COLS)];
    }
  }

  // the second row copies the west and east regions
  if (l_row == 1 && col < SCL_COLS) {
    // Fill columns of local memory left of the mapped elements when padding
    // elements to the left are needed
    if (col < SCL_OVERLAP_RANGE) {
      for (i = -SCL_OVERLAP_RANGE;
           i < (int)get_local_size(1) + SCL_OVERLAP_RANGE; i++) {
#ifdef NEUTRAL
        SCL_SHARED[localIndex(i - 1, -SCL_OVERLAP_RANGE)] = NEUTRAL;
#else // NEAREST
        SCL_SHARED[localIndex(i - 1, -SCL_OVERLAP_RANGE)] =
            SCL_IN[globalIndex(i - 1, -col, SCL_COLS)];
#endif
      }
    }
    // Fill columns of local memory left of the mapped elements
    else if (l_col < SCL_OVERLAP_RANGE) {
      for (i = -SCL_OVERLAP_RANGE;
           i < (int)get_local_size(1) + SCL_OVERLAP_RANGE; i++) {
        SCL_SHARED[localIndex(i - 1, -SCL_OVERLAP_RANGE)] =
            SCL_IN[globalIndex(i - 1, -SCL_OVERLAP_RANGE, SCL_COLS)];
      }
    }

    // Fill columns of local memory right of the mapped elements when padding
    // elements to the right are needed
    if (col >= (SCL_COLS - SCL_OVERLAP_RANGE)) {
      for (i = -SCL_OVERLAP_RANGE;
           i < (int)get_local_size(1) + SCL_OVERLAP_RANGE; i++) {
#ifdef NEUTRAL
        SCL_SHARED[localIndex(i - 1, +SCL_OVERLAP_RANGE)] = NEUTRAL;
#else // NEAREST
        SCL_SHARED[localIndex(i - 1, +SCL_OVERLAP_RANGE)] =
            SCL_IN[globalIndex(i - 1, +(SCL_COLS - col - 1), SCL_COLS)];
#endif
      }
    }
    // Fill columns of local memory right of the mapped elements
    else if (l_col >= (int)(get_local_size(0) - SCL_OVERLAP_RANGE)) {
      for (i = -SCL_OVERLAP_RANGE;
           i < (int)get_local_size(1) + SCL_OVERLAP_RANGE; i++) {
        SCL_SHARED[localIndex(i - 1, +SCL_OVERLAP_RANGE)] =
            SCL_IN[globalIndex(i - 1, +SCL_OVERLAP_RANGE, SCL_COLS)];
      }
    }
  }

  barrier(CLK_LOCAL_MEM_FENCE);

  if (row < SCL_ELEMENTS / SCL_COLS && col < SCL_COLS) {
    SCL_OUT[globalIndex(0, 0, SCL_COLS)] = USR_FUNC(Mm);
  }
}
)"

