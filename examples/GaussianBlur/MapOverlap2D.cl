/*****************************************************************************
 * Copyright (c) 2011-2012 The skelcl Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of skelcl.                                              *
 * skelcl is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * skelcl is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * skelcl is distributed in the hope that it will be useful,                 *
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

int func(input_matrix_t img, __global float* kernelVec, int range)
{
  float sum = 0.0f;
  float norm = 0.0f;
  for (int i = -range; i <= range; i++) {
    for (int j = -range; j <= range; j++) {
      sum  += getData(img, i, j) * kernelVec[(i + j) / 2 + range];
      norm += kernelVec[(i + j) / 2 + range];
    }
  }
  float v = sum / norm;
  return (v > 255) ? 255 : ((v < 0) ? 0 : v);
}

