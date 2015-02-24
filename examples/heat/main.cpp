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

#include <chrono>

using namespace skelcl;

long long get_time() {
    auto time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
      time.time_since_epoch()).count();
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

	long long time4;

	using namespace pvsutil::cmdline;
	pvsutil::CLArgParser cmd(Description("Computation of the Heat equation."));

	auto deviceCount = Arg<int>(Flags(Long("device_count")),
			Description("Number of devices used by SkelCL."), Default(1));

	auto deviceType = Arg<device_type>(Flags(Long("device_type")),
			Description("Device type: ANY, CPU, "
					"GPU, ACCELERATOR"), Default(device_type::ANY));

	auto iterationen = Arg<int>(Flags(Long("iterationen")),
			Description("The number of iterations"), Default(1));
	auto iterationenBetweenSwaps = Arg<int>(Flags(Long("iterationenSwap")),
			Description("The number of iterations between Swaps"), Default(-1));

	auto problemsize = Arg<int>(Flags(Long("problemsize")),
			Description("Size of one side of the grid"), Default(1024));

	cmd.add(&deviceCount, &deviceType, &problemsize, &iterationen,
			&iterationenBetweenSwaps);
	cmd.parse(argc, argv);

	std::stringstream out("_");

	out << problemsize << "_dev" << deviceCount << "_iter" << iterationen
			<< ".pgm";

	//Read pgm-File
	std::vector<float> img(1);
	img.clear();
	for (int i = 0; i < problemsize * problemsize; i++) {
		img.push_back(0);
	}
	for (int i = problemsize / 2; i < problemsize / 2 + sqrt(problemsize);
			i++) {
		for (int j = problemsize / 2; j < problemsize / 2 + sqrt(problemsize);
				j++) {
			img[i * problemsize + j] = 255;
		}
	}

	time0 = get_time();

	skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

	Matrix<float> inputImage(img, problemsize);

	time1 = get_time();

	skelcl::MapOverlap<float(float)> s(std::ifstream { "./heat.cl" }, 1,
			detail::Padding::NEUTRAL, 0, "func");

	for (int iter = 0; iter < iterationen; iter++) {
		inputImage = s(inputImage);
		inputImage.copyDataToHost();
		inputImage.resize(inputImage.size());
	}

	Matrix<float>::iterator itr = inputImage.begin();

	//Get time
	time4 = get_time();
	printf("Total time : %.12f\n", (float) (time4 - time0) / 1000000);
	printf("Total without init time : %.12f\n",
			(float) (time4 - time1) / 1000000);

	//writePPM(outputImage, out.str());

	skelcl::terminate();

}
