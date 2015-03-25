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

#include <cstdlib>
#include <ctime>

#include <chrono>
#include <iostream>
#include <algorithm>
#include <numeric>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/Zip.h>

using namespace skelcl;

const double epsilon = 1e-12;

long long getTime()
{
    auto time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        time.time_since_epoch()).count();
}

template <typename ForwardIterator>
void fillVector(ForwardIterator begin, ForwardIterator end)
{
    srand((unsigned)time(0));
    while (begin != end) {
        *begin = ((float)rand() / (float)RAND_MAX) * 125.0f;
        ++begin;
    }
}

float fillScalar()
{
    srand((unsigned)time(0));
    return ((float)rand() / (float)RAND_MAX) * 125.0f;
}

long long saxpy(int size, bool checkResult)
{
    // Y <- a * X + Y
    Zip<float(float, float)> saxpy("float func(float x, float y, float a){"
                                   "  return a*x + y;"
                                   "}");

    Vector<float> X(size);
    Vector<float> Y(size);
    float a = fillScalar();

    fillVector(X.begin(), X.end());
    fillVector(Y.begin(), Y.end());

    Vector<float> Y_orig(Y); // copy Y for verification later

    auto start = getTime();
    Y = saxpy( X, Y, a );
    Y.copyDataToHost();
    auto end = getTime();

    if (checkResult) {
        int errors = 0;
        for (int i = 0; i < size; ++i) {
            if ( fabs(Y[i] - (a * X[i] + Y_orig[i])) > epsilon ) {
                errors++;
            }
        }
        if (errors > 0) {
            LOG_ERROR(errors, " errors detected.");
        }
    }

    return end - start;
}

int main(int argc, char** argv)
{
    using namespace pvsutil::cmdline;
    pvsutil::CLArgParser cmd(Description("Computation of the mandelbrot set."));

    // Parse arguments.
    auto verbose = Arg<bool>(Flags(Short('v'), Long("verbose")),
                             Description("Enable verbose logging."),
                             Default(false));

    auto size = Arg<size_t>(Flags(Short('n'), Long("size")),
                            Description("Size of the two vectors used in "
                                        "the computation."),
                            Default<size_t>(1024 * 1024));

    auto deviceCount = Arg<int>(Flags(Long("device-count")),
                                Description("Number of devices used by SkelCL."),
                                Default(1));

    auto deviceType = Arg<device_type>(Flags(Long("device-type")),
                                       Description("Device type: ANY, CPU, "
                                                   "GPU, ACCELERATOR"),
                                       Default(device_type::ANY));

    auto checkResult = Arg<bool>(Flags(Short('c'), Long("check")),
                                 Description("Check the computed result "
                                             "against a sequential computed "
                                             "version."),
                                 Default(false));

    cmd.add(&verbose, &size, &deviceCount, &deviceType, &checkResult);
    cmd.parse(argc, argv);

    if (verbose) {
        pvsutil::defaultLogger.setLoggingLevel(
            pvsutil::Logger::Severity::DebugInfo);
    }

    skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));
    long long time = saxpy(size, checkResult);

    printf("Size:         %lu\n", size.getValue());
    printf("Elapsed time: %lld ms\n", time);

    return 0;
}
