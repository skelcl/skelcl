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
    long long time2;
    long long time3;
    long long time4;
    long long time5;
    long long time6;
    long long time7;
    int i;

    using namespace pvsutil::cmdline;
    pvsutil::CLArgParser cmd(Description("Computation of the Gaussian blur."));

    auto deviceCount = Arg<int>(Flags(Long("device_count")),
            Description("Number of devices used by SkelCL."), Default(1));

    auto deviceType = Arg<device_type>(Flags(Long("device_type")),
            Description("Device type: ANY, CPU, "
                    "GPU, ACCELERATOR"), Default(device_type::GPU));

    auto range = Arg<int>(Flags(Long("range")),
            Description("The Overlap radius"), Default(5));

    auto inFile = Arg<std::string>(Flags(Long("inFile")),
                                       Description("Filename of the input file"), Default(std::string("lena.pgm")));

    cmd.add(&deviceCount, &deviceType, &range, &inFile);
    cmd.parse(argc, argv);

    std::stringstream out("_");

    out << static_cast<std::string>(inFile).substr(0, static_cast<std::string>(inFile).find(".")) << "_" << range << "_devs_" << deviceCount << ".pgm";


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

    time1 = get_time();

    skelcl::MapOverlap<float(float)> m(std::ifstream { "./cannyGauss.cl" }, static_cast<unsigned int>(range),
                        detail::Padding::NEUTRAL, 255, "func");
    skelcl::MapOverlap<float(float)> n(std::ifstream { "./cannySobel.cl" }, 1,
                        detail::Padding::NEAREST, 0, "func");
    skelcl::MapOverlap<float(float)> o(std::ifstream { "./cannyNMS.cl" }, 1,
                        detail::Padding::NEUTRAL, 0, "func");
    skelcl::MapOverlap<float(float)> p(std::ifstream { "./cannyThreshold.cl" }, 1,
                        detail::Padding::NEAREST, 255, "func");

    time2 = get_time();

    Matrix<float> outputImage = m(inputImage, kernelVec, static_cast<unsigned int>(range));
    outputImage.copyDataToHost();
    outputImage.resize(inputImage.size());


    //Get time
    time3=get_time();

    Matrix<float> tempImage = n(outputImage, kernelVec, 1);
    tempImage.copyDataToHost();
    tempImage.resize(inputImage.size());

    //Get time
    time4=get_time();

    outputImage = o(tempImage, kernelVec, 1);
    outputImage.copyDataToHost();
    outputImage.resize(inputImage.size());

    //Get time
    time5=get_time();

    tempImage = p(outputImage, kernelVec, 1);

    //Get time
    time6=get_time();

    Matrix<float>::iterator itr = tempImage.begin();

    //Get time
    time7=get_time();
    printf("Total Init: %.12f\n", (float) (time1-time0) / 1000000);
    printf("Total Creation: %.12f\n", (float) (time2-time1) / 1000000);
    printf("Total Gauß: %.12f\n", (float) (time3-time2) / 1000000);
    printf("Total Sobel: %.12f\n", (float) (time4-time3) / 1000000);
    printf("Total NSM: %.12f\n", (float) (time5-time4) / 1000000);
    printf("Total Threshold: %.12f\n", (float) (time6-time5) / 1000000);
    printf("Total Total: %.12f\n", (float) (time6-time0) / 1000000);
    printf("Total Total no init: %.12f\n", (float) (time6-time1) / 1000000);

    writePPM(tempImage, out.str());

    skelcl::terminate();

}
