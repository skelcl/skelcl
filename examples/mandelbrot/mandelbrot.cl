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
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#define COLOR_R(n) ((n & 63) << 2)
#define COLOR_G(n) ((n << 3) & 255)
#define COLOR_B(n) ((n >> 8) & 255)

int iterate(float x, float y)
{
  int n = 0;
  float r = 0.0f, s = 0.0f;
  float rNext = 0.0f;
  
  while (((r * r) + (s * s) <= 4.0f) && (n < ITERATIONS)) {
    rNext = ((r * r) - (s * s)) + x;
    s = (2 * r * s) + y;
    r = rNext;
    ++n;
  }
  return n;
}

Pixel func(IndexPoint position,
           float startX, float startY,
           float dx, float dy)
{
  float x = startX + position.x * dx;
  float y = startY + position.y * dy;

  int n = iterate(x, y);
  
  Pixel pixel;
  if (n == ITERATIONS) {
    pixel.r = 0;
    pixel.g = 0;
    pixel.b = 0;
  } else {
    pixel.r = COLOR_R(n);
    pixel.g = COLOR_G(n);
    pixel.b = COLOR_B(n);
  }
  return pixel;
}

