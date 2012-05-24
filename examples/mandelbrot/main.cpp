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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/Map.h>
#include <SkelCL/detail/Logger.h>

typedef struct {
	unsigned short x;
	unsigned short y;
} Position;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Pixel;

#define ITERATIONS 100

#define COLOR_R(n) ((n & 63) << 2)
#define COLOR_G(n) ((n << 3) & 255)
#define COLOR_B(n) ((n >> 8) & 255)

#define WIDTH 4096
#define HEIGHT 3072

#define CENTER_X -0.73
#define CENTER_Y -0.16
#define ZOOM 27615


using namespace skelcl;

template <typename Iterator>
void initPositions(Iterator iter)
{
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      iter->x = x;
      iter->y = y;
      ++iter;
    }
  }
}

std::ostream& operator<< (std::ostream& out, Pixel p)
{
  out << p.r << p.g << p.b;
  return out;
}

template <typename Iterator>
void writePPM (Iterator first, Iterator last, const std::string& filename)
{
  std::ofstream outputFile(filename.c_str());

  outputFile << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

  std::copy(first, last, std::ostream_iterator<Pixel>(outputFile));
}

void mandelbrot()
{
  Vector<Position> input(WIDTH * HEIGHT);

  initPositions(input.begin());

  Map<Pixel(Position)> m(std::ifstream("mandelbrot.cl"));

  float startX = CENTER_X - ((float) WIDTH / (ZOOM * 2));
  float endX = CENTER_X + ((float) WIDTH / (ZOOM * 2));

  float startY = CENTER_Y - ((float) HEIGHT / (ZOOM * 2));
  float endY = CENTER_Y + ((float) HEIGHT / (ZOOM * 2));

  float dx = (endX - startX) / WIDTH;
  float dy = (endY - startY) / HEIGHT;

  Vector<Pixel> output(input.size());
  //m(skelcl::out(output), input, startX, startY, dx, dy);
  output = m(input, startX, startY, dx, dy);

  writePPM(output.begin(), output.end(), "mandelbrot.ppm");
}

int main()
{
  //skelcl::detail::defaultLogger.setLoggingLevel(skelcl::detail::Logger::Severity::Debug);
  skelcl::init(2);
  mandelbrot();
  return 0;
}

