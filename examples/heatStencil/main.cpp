/*
 * glaettenMapOverlap.cpp
 *
 *  Created on: 02.03.2011
 *      Author: Stefan Breuer
 */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/IndexMatrix.h>
#include <SkelCL/MapOverlap.h>
#include <SkelCL/Stencil.h>
#include <SkelCL/detail/Padding.h>

using namespace skelcl;

long long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

void writePPM(Matrix<float>& img, const std::string filename) {
    std::ofstream outputFile(filename.c_str());

    outputFile << "P2\n" << "#Creator: sbr\n" << img.columnCount() << " "
            << img.rowCount() << "\n255\n";

    Matrix<float>::iterator itr;

    for (itr = img.begin(); itr != img.end(); ++itr) {
        outputFile << (int) *itr << "\n";
    }
}

void writePPM(Matrix<int>& img, const std::string filename) {
    std::ofstream outputFile(filename.c_str());
    outputFile << "P2\n" << "#Creator: sbr\n" << img.columnCount() << " "
            << img.rowCount() << "\n255\n";
    Matrix<int>::iterator itr = img.begin();
    for (itr = img.begin(); itr != img.end(); ++itr) {
        outputFile << *itr << "\n";
    }
}

void writePPM(std::vector<float>& img, const std::string filename) {
    std::ofstream outputFile(filename.c_str());

    outputFile << "P2\n" << "#Creator: sbr\n" << 640 << " " << 400 << "\n255\n";

    std::vector<float>::iterator itr;
    for (itr = img.begin(); itr != img.end(); ++itr) {
        outputFile << *itr << "\n";
    }
}

int readPPM(const std::string inFile, std::vector<float>& img) {
    img.clear();
    int numrows = 0, numcols = 0;
    std::ifstream infile(inFile);

    std::stringstream ss;
    std::string inputLine = "";

    // First line : version
    getline(infile, inputLine);

    // Second line : comment
    getline(infile, inputLine);

    // Continue with a stringstream
    getline(infile, inputLine);
    std::stringstream ss2(inputLine);
    //	// Third line : size
    ss2 >> numcols >> numrows;

    getline(infile, inputLine);

    int i;
    while (getline(infile, inputLine)) {
        std::stringstream ss(inputLine);
        ss >> i;
        img.push_back(i);
    }

    infile.close();
    return numcols;
}

int main(int argc, char** argv) {
    long long time0;
    long long time1;
    int i;

    using namespace pvsutil::cmdline;
    pvsutil::CLArgParser cmd(Description("Computation of the Gaussian blur."));

    auto deviceCount = Arg<int>(Flags(Long("device_count")),
            Description("Number of devices used by SkelCL."), Default(1));

    auto deviceType = Arg<device_type>(Flags(Long("device_type")),
            Description("Device type: ANY, CPU, "
                    "GPU, ACCELERATOR"), Default(device_type::GPU));

    auto range = Arg<int>(Flags(Long("range")),
            Description("The range in y direction to one side"), Default(5));
    auto iterationen = Arg<int>(Flags(Long("iterationen")),
                                   Description("The number of iterations"), Default(1));
    auto iterationenBetweenSwaps = Arg<int>(Flags(Long("iterationenSwap")),
                                   Description("The number of iterations between Swaps"), Default(-1));

    auto inFile = Arg<std::string>(Flags(Long("inFile")),
                                       Description("Filename of the input file"), Default(std::string("lena.pgm")));

    cmd.add(&deviceCount, &deviceType, &range, &inFile, &iterationen, &iterationenBetweenSwaps);
    cmd.parse(argc, argv);

    std::stringstream out("_");

    out << static_cast<std::string>(inFile).substr(0, static_cast<std::string>(inFile).find(".")) << "_or_" << range << "_devs_" << deviceCount << ".pgm";

    //calculate the kernel
    int fwhm = 5;
    int offset = (2 * range + 1) / 2;

    /*
     * Given as parameter
     * FWHM = 2 sqrt(2 ln2) sigma ~ 2.35 sigma
     */
    float a = (fwhm / 2.354);

    skelcl::Vector<float> kernelVec(2*range+1);

    /* set up kernel to weight the pixels */
    /* (KERNEL_SIZE - offset -1) is the CORRECT version */
    for (i = -offset; i <= ((2 * range + 1) - offset - 1); i++) {
        kernelVec[i + offset] = exp(-i * i / (2 * a * a));
    }

    //Read pgm-File
    std::vector<float> img(1);

    int numcols = readPPM(inFile, img);

    time0 = get_time();

    skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

    Matrix<float> inputImage(img, numcols);

    skelcl::Stencil<float(float)> s(std::ifstream { "./heat.cl" }, static_cast<int>(range),0,static_cast<int>(range),0,
                        detail::Padding::NEAREST_INITIAL, 0, "func", static_cast<int>(iterationenBetweenSwaps));

    Matrix<float> outputImage = s(iterationen, inputImage, kernelVec, static_cast<int>(range));

    Matrix<float>::iterator itr = outputImage.begin();

    //Get time
    time1=get_time();
    printf("Total: %.12f\n", (float) (time1-time0) / 1000000);

    writePPM(outputImage, out.str());

    skelcl::terminate();

}
