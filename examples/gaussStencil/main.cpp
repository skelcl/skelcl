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

int readPPM(const std::string inFile, std::vector<int>& img) {
    img.clear();
    int numrows = 0, numcols = 0;
    std::ifstream infile(inFile);

    std::stringstream ss;
    std::string inputLine = "";

    // First line : version
    getline(infile, inputLine);
    std::cout << "Version : " << inputLine << std::endl;

    // Second line : comment
    getline(infile, inputLine);
    std::cout << "Comment : " << inputLine << std::endl;

    // Continue with a stringstream
    getline(infile, inputLine);
    std::stringstream ss2(inputLine);
    //	// Third line : size
    ss2 >> numcols >> numrows;

    std::cout << numcols << " columns and " << numrows << " rows" << std::endl;

    getline(infile, inputLine);
    std::cout << "Max Value: " << inputLine << std::endl;

//	std::vector<int> img;
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
    int range = 2;
    int i;
    using namespace pvsutil::cmdline;
    pvsutil::CLArgParser cmd(Description("Computation of the Gaussian blur."));

    auto deviceCount = Arg<int>(Flags(Long("device_count")),
            Description("Number of devices used by SkelCL."), Default(1));

    auto deviceType = Arg<device_type>(Flags(Long("device_type")),
            Description("Device type: ANY, CPU, "
                    "GPU, ACCELERATOR"), Default(device_type::GPU));


    //calculate the kernel
    int fwhm = 5;
//	cl_uint *kernel_size = (cl_uint*) malloc(sizeof(cl_uint));
//	kernel_size[0] = (cl_uint) 2 * range + 1;
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
    std::vector<int> img(1);
    std::string inFile("pgm.pgm");
    if (argc > 1) {
        inFile = argv[1];
    }
    int numcols = readPPM(inFile, img);

    skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

    Matrix<int> inputImage(img, numcols);

    skelcl::Stencil<int(int)> s(std::ifstream { "./gauss2DStencil3.cl" }, 2,2,2,2,
                        detail::Padding::NEAREST, 255, "func");

    skelcl::Stencil<int(int)> t(std::ifstream { "./gauss2DStencil2.cl" },0,0,0,0,
                                detail::Padding::NEUTRAL, 0, "func");

    s.add(std::ifstream { "./gauss2DStencil.cl" },1,1,1,1,
        detail::Padding::NEAREST, 0, "func");
    //    LOG_DEBUG("Adding next");
  s.add(std::ifstream { "./gauss2DStencil2.cl" }, 1,1,1,1,
       detail::Padding::NEAREST, 1, "func");
  s.add(std::ifstream { "./gauss2DStencil4.cl" }, 0,0,0,0,
       detail::Padding::NEAREST, 1, "func");

    Matrix<int> outputImage = s(1, inputImage, kernelVec, range);
    //Matrix<int> outputImage = t(1, inputImage);

//	Matrix<int>::iterator itr;
    /*for(itr = outputImage.begin(); itr!=outputImage.end(); itr++){
        //if(*itr>255 || *itr<0)
        std::cout << "VAL: " << *itr << " " << count << std::endl;
        count++;
    }*/
    std::cout << "Matrix " << outputImage.columnCount() << " columns and "
            << outputImage.rowCount() << " rows" << std::endl;
    std::string outFile("out.pgm");
    if (argc > 2) {
        std::cout << "Argv[2] " << argv[2] << std::endl;
        outFile = argv[2];
    }
    LOG_DEBUG("writing");
   writePPM(outputImage, outFile);

    //skelcl::terminate();

}
