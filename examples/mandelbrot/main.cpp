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

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/IndexMatrix.h>
#include <SkelCL/Map.h>

SKELCL_COMMON_DEFINITION(
typedef struct {    \
  unsigned char r;  \
  unsigned char g;  \
  unsigned char b;  \
} Pixel;            \
)

#define ITERATIONS 100

#define WIDTH 4096
#define HEIGHT 3072

#define CENTER_X -0.73
#define CENTER_Y -0.16
#define ZOOM 27615
SKELCL_ADD_DEFINE(ITERATIONS)
SKELCL_ADD_DEFINE(WIDTH)
SKELCL_ADD_DEFINE(HEIGHT)
SKELCL_ADD_DEFINE(CENTER_X)
SKELCL_ADD_DEFINE(CENTER_Y)
SKELCL_ADD_DEFINE(ZOOM)

//SKELCL_ADD_DEFINES(ITERATIONS, WIDTH, HEIGHT, CENTER_X, CENTER_Y, ZOOM)

using namespace skelcl;

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
  IndexMatrix positions({HEIGHT, WIDTH});

  Map<Pixel(IndexPoint)> m(std::ifstream("mandelbrot.cl"));

  float startX = CENTER_X - ((float) WIDTH / (ZOOM * 2));
  float endX = CENTER_X + ((float) WIDTH / (ZOOM * 2));

  float startY = CENTER_Y - ((float) HEIGHT / (ZOOM * 2));
  float endY = CENTER_Y + ((float) HEIGHT / (ZOOM * 2));

  float dx = (endX - startX) / WIDTH;
  float dy = (endY - startY) / HEIGHT;

  Matrix<Pixel> output = m(positions, startX, startY, dx, dy);

  writePPM(output.begin(), output.end(), "mandelbrot.ppm");
}

int main(int argc, char** argv)
{
  using namespace pvsutil::cmdline;
  pvsutil::CLArgParser cmd(Description("Computation of the mandelbrot set."));

  auto deviceCount = Arg<int>(Flags(Long("device_count")),
                              Description("Number of devices used by SkelCL."),
                              Default(2));

  auto deviceType = Arg<device_type>(Flags(Long("device_type")),
                                     Description("Device type: ANY, CPU, "
                                                 "GPU, ACCELERATOR"),
                                     Default(device_type::ANY));

  auto enableLogging = Arg<bool>(Flags(Short('l'), Long("logging"),
                                       Long("verbose_logging")),
                                 Description("Enable verbose logging."),
                                 Default(false));

  cmd.add(deviceCount, deviceType, enableLogging)
     .parse(argc, argv);

  if (enableLogging) {
    pvsutil::defaultLogger.setLoggingLevel(
        pvsutil::Logger::Severity::DebugInfo);
  }

  skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));
  mandelbrot();
  return 0;
}
