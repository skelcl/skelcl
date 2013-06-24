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
/// \author Stefan Breuer <s_breu03@uni-muenster.de>
///
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/MapOverlap.h>

#include "Test.h"

class MapOverlapTests: public ::testing::Test {
protected:
	MapOverlapTests() {
		//pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
		skelcl::init(skelcl::nDevices(1));

		srand(1);
	}

	~MapOverlapTests() {
		skelcl::terminate();
	}
};

TEST_F(MapOverlapTests, CreateMapOverlapWithString) {
	skelcl::MapOverlap<float(float)> s(
			"float func(__local float* img) {return getElem2D(img,0,0);}", 1,
			skelcl::detail::Padding::NEAREST, 0);
}
TEST_F(MapOverlapTests, CreateMapOverlapWithFile) {
	skelcl::MapOverlap<float(float)> s(std::ifstream { "gauss2D.cl" }, 1,
			skelcl::detail::Padding::NEAREST, 0);
}

// Test additional arguments
TEST_F(MapOverlapTests, RegularMapOverlapTest) {
	skelcl::MapOverlap<float(float)> m(
			"float func(__local float* img) {return getElem2D(img,0,0);}", 0,
			skelcl::detail::Padding::NEAREST, 0);

	std::vector<float> tmpIn(4096);
	for (size_t i = 0; i < tmpIn.size(); ++i)
		tmpIn[i] = rand() % 100;
	EXPECT_EQ(4096, tmpIn.size());

	skelcl::Matrix<float> in(tmpIn, 64);
	EXPECT_EQ(64, in.rowCount());
	EXPECT_EQ(64, in.columnCount());

	skelcl::Matrix<float> output = m(in);
	EXPECT_EQ(64, output.rowCount());
	EXPECT_EQ(64, output.columnCount());

	for (size_t i = 0; i < output.rowCount(); ++i) {
		for (size_t j = 0; j < output.columnCount(); ++j) {
			EXPECT_EQ(in[i][j], output[i][j]);
		}
	}
}

// Test additional arguments
TEST_F(MapOverlapTests, MapOverlapAdditionalArguments) {
	skelcl::MapOverlap<float(float)> m(
			"float func(__local float* img, int a) {return getElem2D(img,0,0) + a;}",
			0, skelcl::detail::Padding::NEAREST, 0);

	std::vector<float> tmpIn(4096);
	for (size_t i = 0; i < tmpIn.size(); ++i)
		tmpIn[i] = rand() % 100;
	EXPECT_EQ(4096, tmpIn.size());

	skelcl::Matrix<float> in(tmpIn, 64);
	EXPECT_EQ(64, in.rowCount());
	EXPECT_EQ(64, in.columnCount());

	int a = 5;
	skelcl::Matrix<float> output = m(in, a);
	EXPECT_EQ(64, output.rowCount());
	EXPECT_EQ(64, output.columnCount());

	for (size_t i = 0; i < output.rowCount(); ++i) {
		for (size_t j = 0; j < output.columnCount(); ++j) {
			EXPECT_EQ(in[i][j] + a, output[i][j]);
		}
	}
}
