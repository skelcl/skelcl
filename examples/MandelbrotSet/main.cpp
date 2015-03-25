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
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/IndexMatrix.h>
#include <SkelCL/Map.h>

SKELCL_COMMON_DEFINITION(
    typedef struct {      \
        unsigned char r;  \
        unsigned char g;  \
        unsigned char b;  \
    } Pixel;              \
)

#define CENTER_X -0.73
#define CENTER_Y -0.16
#define ZOOM 27615
SKELCL_ADD_DEFINE(CENTER_X)
SKELCL_ADD_DEFINE(CENTER_Y)
SKELCL_ADD_DEFINE(ZOOM)

using namespace skelcl;

long long getTime()
{
    auto time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        time.time_since_epoch()).count();
}

std::ostream& operator<< (std::ostream& out, Pixel p)
{
    out << p.r << p.g << p.b;
    return out;
}

template <typename Iterator>
void writePPM (Iterator first, Iterator last,
               const size_t width, const size_t height,
               const std::string& filename)
{
    std::ofstream outputFile(filename.c_str());

    outputFile << "P6\n" << width << " " << height << "\n255\n";

    std::copy(first, last, std::ostream_iterator<Pixel>(outputFile));
}

void mandelbrot(const unsigned int iterations,
                const size_t width, const size_t height,
                const std::string outFile)
{
    IndexMatrix positions({height, width});

    Map<Pixel(IndexPoint)> m(std::ifstream("mandelbrot.cl"));

    float startX = CENTER_X - ((float) width / (ZOOM * 2));
    float endX = CENTER_X + ((float) width / (ZOOM * 2));

    float startY = CENTER_Y - ((float) height / (ZOOM * 2));
    float endY = CENTER_Y + ((float) height / (ZOOM * 2));

    float dx = (endX - startX) / width;
    float dy = (endY - startY) / height;

    Matrix<Pixel> output = m(positions, startX, startY, dx, dy, iterations);

    writePPM(output.begin(), output.end(), width, height, outFile);
}

int main(int argc, char** argv)
{
    using namespace pvsutil::cmdline;

    pvsutil::CLArgParser cmd(Description("Computation of the mandelbrot set."));

    // Parse arguments.
    auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                             Description("Enable verbose logging."),
                             Default(false));

    auto iterations = Arg<size_t>(Flags(Short('i'), Long("iterations")),
                                  Description("Number of iterations"),
                                  Default<size_t>(100));

    auto width = Arg<size_t>(Flags(Short('w'), Long("width")),
                             Description("Width of set"),
                             Default<size_t>(4096));
    auto height = Arg<size_t>(Flags(Short('h'), Long("height")),
                              Description("Height of set"),
                              Default<size_t>(3072));

    auto outFile = Arg<std::string>(Flags(Short('o'), Long("output")),
                                    Description("Path of the output PPM file"),
                                    Default(std::string("data/mandelbrot.ppm")));

    auto deviceCount = Arg<int>(Flags(Long("device-count")),
                                Description("Number of devices used by SkelCL."),
                                Default(1));

    auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                       Description("Device type: ANY, CPU, "
                                                   "GPU, ACCELERATOR"),
                                       Default(device_type::ANY));

    auto enableLogging = Arg<bool>(Flags(Short('l'), Long("logging"),
                                         Long("verbose_logging")),
                                   Description("Enable verbose logging."),
                                   Default(false));

    cmd.add(&verbose, &iterations, &width, &height, &outFile,
            &deviceCount, &deviceType);
    cmd.parse(argc, argv);

    if (verbose) {
        pvsutil::defaultLogger.setLoggingLevel(
            pvsutil::Logger::Severity::DebugInfo);
    }

    skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

    auto start = getTime();
    mandelbrot(iterations, width, height, outFile);
    auto end = getTime();

    printf("Iterations:   %lu\n", iterations.getValue());
    printf("Image size:   %lu x %lu\n", width.getValue(), height.getValue());
    printf("Elapsed time: %lld ms\n", end - start);
    printf("Output:       %s\n", outFile.getValue().c_str());

    return 0;
}
